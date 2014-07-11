#include <net/ether.h>
#include <net/ip.h>
#include <net/checksum.h>

void ip_pack(Packet* packet, uint16_t ip_body_len) {
	Ether* ether = (Ether*)(packet->buffer + packet->start);
	IP* ip = (IP*)ether->payload;
	
	ip->length = endian16(ip->ihl * 4 + ip_body_len);
	ip->ttl = IP_TTL;
	ip->checksum = 0;
	
	ip->checksum = endian16(checksum(ip, ip->ihl * 4));
}