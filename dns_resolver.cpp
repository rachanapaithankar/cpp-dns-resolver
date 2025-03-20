#define _WIN32_WINNT 0x0600  // Ensure Windows 7+ API availability

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>
#include <string>
#include <stdexcept>
#include <limits>
#include <memory>

// Links the Winsock2 library for networking functions
#pragma comment(lib, "Ws2_32.lib")

// RAII wrapper to initialize and clean up Winsock automatically
class WinsockInitializer
{
public:
  // Constructor
  WinsockInitializer()
  {
    // Starts Winsock v2.2; throws an error if initialization fails
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
      {
        throw std::runtime_error("WSAStartup failed.");
      }
  }

  // Destructor
  ~WinsockInitializer()
  {
    WSACleanup();
  }

private:
  // Structure to hold Winsock data
  WSADATA wsaData;
};

// This class provides methods to resolve domain names to IP addresses and perform reverse DNS lookups
// It uses the Winsock API to fetch network information
class DNSResolver
{
public:

  /**
   * Resolves the given domain name to its IP addresses.
   * @param[in] domain The domain name to resolve (e.g., "google.com").
   * @param[in] family Address family (AF_INET for IPv4, AF_INET6 for IPv6, AF_UNSPEC for both).
   */
  void resolveDNS(const std::string& domain, int family = AF_UNSPEC)
  {
    std::cout << "\nResolving: " << domain << "\n";
    struct addrinfo hints = {}, *res = nullptr;

    // Specifies whether to resolve for IPv4, IPv6, or both
    hints.ai_family = family;

    // Sets the socket type to TCP
    hints.ai_socktype = SOCK_STREAM;

    // Perform DNS lookup
    int status = getaddrinfo(domain.c_str(), nullptr, &hints, &res);
    if (status != 0)
      {
        std::cerr << "Error: Could not resolve " << domain << ". " << gai_strerror(status) << "\n";
        return;
      }

    // Automatically frees addrinfo to prevent memory leaks and ensure exception safety
    std::unique_ptr<struct addrinfo, decltype(&freeaddrinfo)> res_ptr(res, freeaddrinfo);

    // Buffer to store the resolved IP address
    char ipStr[NI_MAXHOST];

    std::cout << "Addresses:\n";

    // Iterate through all resolved addresses and print them
    for (struct addrinfo* p = res_ptr.get(); p != nullptr; p = p->ai_next)
      {
        // Variable to hold the length of the IP string
        DWORD ipStrLen = NI_MAXHOST;

        // Convert the resolved address into a human-readable format
        if (WSAAddressToStringA(p->ai_addr, p->ai_addrlen, nullptr, ipStr, &ipStrLen) == 0)
          {
            std::cout << "  " << ipStr << "\n";
          }
        else
          {
            std::cerr << "Warning: Failed to convert address. Skipping...\n";
          }
      }
  }

  /**
   * Perform a reverse DNS lookup to find the hostname for a given IP address.
   * @param[in] ip The IPv4 address to resolve.
   */
  void reverseDNSLookup(const std::string& ip)
  {
    std::cout << "\nReverse Lookup: " << ip << "\n";

    // Structure to store the IP address information
    struct sockaddr_in sa;

    // Specify that the address belongs to the IPv4 family
    sa.sin_family = AF_INET;

    // Convert the IP string to a network address format
    sa.sin_addr.s_addr = inet_addr(ip.c_str());

    // Validate if the IP format is correct
    if (sa.sin_addr.s_addr == INADDR_NONE)
      {
        std::cerr << "Invalid IP format. Please enter a valid IPv4 address.\n";
        return;
      }

    char host[NI_MAXHOST];

    // Perform reverse DNS lookup to get the domain name
    if (getnameinfo((struct sockaddr*)&sa, sizeof(sa), host, NI_MAXHOST, nullptr, 0, NI_NAMEREQD) == 0)
      {
        std::cout << "Resolved Hostname: " << host << "\n";
      }
    else
      {
        std::cerr << "Reverse lookup failed for " << ip << "\n";
      }
  }

  /**
   * Resolves multiple domain names sequentially.
   * @param[in] domains A list of domain names to resolve.
   * @param[in] family Address family (IPv4, IPv6, or both).
   */
  void resolveMultipleDomains(const std::vector<std::string>& domains, int family = AF_UNSPEC)
  {
    for (const auto& domain : domains)
      {
        resolveDNS(domain, family);
      }
  }
};

// This class handles user input, ensuring valid numerical input and choices
// It provides methods for selecting an option and choosing an address family
class UserInputHandler
{
public:

  /**
   * Prompts the user to enter a numerical choice.
   * Ensures valid integer input and prevents non-numeric or invalid characters.
   *
   * @return The validated integer choice entered by the user.
   */
  int getUserChoice()
  {
    int choice;
    while (true)
      {
        // Display Options and Prompt the user for input
        try
        {
            std::cin >> choice;

            if (std::cin.fail())
              {
                throw std::invalid_argument("Invalid input. Please enter a number.");
              }

            // Clear any remaining input in the buffer to prevent unintended behavior
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return choice;
        }
        // Handle invalid input by resetting the input stream and displaying an error message
        catch (const std::exception& e)
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cerr << e.what() << "\n";
        }
      }
  }

  /**
   * Prompts the user to select an address family for network communication.
   * Valid options:
   *   1 - IPv4 only
   *   2 - IPv6 only
   *   3 - Both (default)
   *
   * @return The corresponding address family (AF_INET for IPv4, AF_INET6 for IPv6, AF_UNSPEC for both).
   */
  int getFamilyChoice()
  {
    int family;
    while (true)
      {
        // Display options and prompt the user to select an address family
        try
        {
            std::cout << "Select Address Family:\n";
            std::cout << "1. IPv4 only\n";
            std::cout << "2. IPv6 only\n";
            std::cout << "3. Both (default)\n";
            std::cout << "Enter choice: ";
            std::cin >> family;

            if (std::cin.fail() || (family < 1 || family > 3))
              {
                throw std::invalid_argument("Invalid input. Enter 1, 2, or 3.");
              }

            // Clear any extra input and return the corresponding address family
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (family == 1) return AF_INET;
            if (family == 2) return AF_INET6;
            return AF_UNSPEC;

        }
        // Handle invalid input by resetting the input stream and displaying an error message
        catch (const std::exception& e)
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cerr << e.what() << "\n";
        }
      }
  }
};

int main()
{
  try
  {
      WinsockInitializer winsock;  // RAII ensures WSACleanup is called
      DNSResolver resolver;
      UserInputHandler inputHandler;

      // Display menu options for the user
      std::cout << "1. Resolve Domain\n";
      std::cout << "2. Reverse DNS Lookup\n";
      std::cout << "3. Resolve Multiple Domains\n";
      std::cout << "Choose an option: ";

      // Get user choice and validate input
      int choice = inputHandler.getUserChoice();

      // Get address family preference (IPv4, IPv6, or both)
      if (choice == 1)
        {
          // Resolve a single domain name
          std::string domain;
          std::cout << "Enter domain: ";
          std::getline(std::cin, domain);

          // Get address family preference (IPv4, IPv6, or both)
          int family = inputHandler.getFamilyChoice();
          resolver.resolveDNS(domain, family);
        } 
      else if (choice == 2)
        {
          // Perform reverse DNS lookup for an IP address
          std::string ip;
          std::cout << "Enter IP address: ";
          std::getline(std::cin, ip);
          resolver.reverseDNSLookup(ip);
        } 
        else if (choice == 3)
        {
            // Resolve multiple domain names
            int count;
            std::cout << "Enter number of domains: ";
            count = inputHandler.getUserChoice();
        
            std::vector<std::string> domains(count);
        
            // Collect domain names from the user
            for (int i = 0; i < count; ++i)
            {
                std::cout << "Enter domain " << (i + 1) << ": ";
                std::getline(std::cin, domains[i]);
            }
        
            // Get address family preference
            int family = inputHandler.getFamilyChoice();
            resolver.resolveMultipleDomains(domains, family);
        }        
      else
        {
          std::cerr << "Invalid choice. Exiting.\n";
        }
  }
  catch (const std::exception& e)
  {
      // Handle any exceptions thrown during execution
      std::cerr << "Error: " << e.what() << "\n";
  }

  return 0;
}
