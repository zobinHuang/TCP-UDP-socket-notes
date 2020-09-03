# **TCP&UDP Socket Programming Notes**

## **Preface**
This repo contains codes for network communication programs which were wrote during the undergraduate crouse ***Design of Network Software*** taught by Prof. Duan in 2020 spring semester (online) in UESTC. You can use these codes as a basic tutorial for knowing how to use TCP/UDP sockets and design the framework of different types of network communication software such as online chatting, file transferring, etc. The only socket we discussed and showed in this repo is TCP/UDP socket (Layer 4). To better understand all these codes, it will be perfect that you had learn basic knowledge of TCP/IP stack before you check them. All the codes were wrote in C and can be compiled and run on Windows & Linux environment.

## **Structure**
This repo can be divided into two parts and they're arranged as below:
* **PART 1: Basic Notes of Socket**
    * [**Section 1: Socket**](https://github.com/zobinHuang/TCP-UDP-socket-notes/tree/master/1.create_socket)
      * What is a **socket**? 
      * What is the relationship among **socket**, **socket address**, **port** and **ip address**?
      * How to create a socket under Windows and Linux?
    * [**Section 2: C/S Model**](https://github.com/zobinHuang/TCP-UDP-socket-notes/tree/master/2.cs_model) 
      * What is **Client/Server Model** and why? 
      * What **Connection-oriented** and **Connectionless-oriented** refer to? 
      * What is the difference between **Stream Service** and **Datagram Service**?
      * How to use basic socket-related interface functions?
      * Write connction-oriented and connectionless-oriented C/S programs for basic string transferring!
    * **Section 3: Shutdown of Connection-oriented Service**
      * "Rough Closure"
      * Closing by High level Consultation
      * "Elegant Closing"
* **PART 2: Framework of Network Communication Software**
* **PART 3: Extension and Examples**