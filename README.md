# DNS Resolver and Reverse Lookup Tool
 
This is a **C++** command-line tool that performs **DNS resolution** and **reverse DNS lookup** using the Winsock API on **Windows**. It allows users to resolve domain names to IP addresses and perform reverse lookups for IPv4 addresses.
 
## 🚀 Features
 
### ✅ Resolve a single domain to its corresponding IP addresses (IPv4/IPv6).  
### ✅ Perform reverse DNS lookup for an IPv4 address.  
### ✅ Resolve multiple domains in sequence.  
### ✅ Supports both IPv4 and IPv6.  
### ✅ RAII-based Winsock initialization for clean resource management.  
### ✅ Robust input validation to prevent invalid user entries.  
 
## 🛠️ Prerequisites
 
### Windows 7 or later (Supports Winsock2)  
### MinGW or Microsoft Visual Studio (MSVC) for compilation  
### C++11 or later  
 
## 🔧 Compilation & Usage
 
### Compile with MinGW
 
g++ -o dns_resolver.exe dns_resolver.cpp -lws2_32
 
### Run the Program
 
.\dns_resolver.exe
 
## 📖 Usage Instructions
 
1. Resolve Domain  
2. Reverse DNS Lookup  
3. Resolve Multiple Domains  
 
### Domain Resolution Example
 
Enter domain: google.com  
Select Address Family:  
1. IPv4 only  
2. IPv6 only  
3. Both (default)  
Enter choice: 1  
 
Resolving: google.com  
Addresses:  
142.250.190.78  
 
### Reverse DNS Lookup Example
 
Enter IP address: 8.8.8.8  
Reverse Lookup: 8.8.8.8  
Resolved Hostname: dns.google  

## 📜 Open Source Notice

This project does not use any third-party open-source code. It relies only on Windows Winsock APIs.

## 📂 File Structure
 
/dns-resolver  
│── dns_resolver.cpp   # Main source file  
│── README.md          # Project documentation  
