#include <pcap.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <string.h>
#include <iostream>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define SIZE_ETHERNET 14

using namespace std;

struct sniff_ip 
{
    u_char  ip_vhl;                 /* version << 4 | header length >> 2 */
    u_char  ip_tos;                 /* type of service */
    u_short ip_len;                 /* total length */
    u_short ip_id;                  /* identification */
    u_short ip_off;                 /* fragment offset field */
    #define IP_RF 0x8000            /* reserved fragment flag */
    #define IP_DF 0x4000            /* dont fragment flag */
    #define IP_MF 0x2000            /* more fragments flag */
    #define IP_OFFMASK 0x1fff       /* mask for fragmenting bits */
    u_char  ip_ttl;                 /* time to live */
    u_char  ip_p;                   /* protocol */
    u_short ip_sum;                 /* checksum */
    struct  in_addr ip_src,ip_dst;  /* source and dest address */
};

int cnt = 1;
int gre1_ex = 0;
int gre1_ex_pre = 0;
int gre2_ex = 0;
int gre2_ex_pre = 0;
char filter[200];
pcap_t *handle=NULL;

static void pcap_callback1(u_char *arg, const struct pcap_pkthdr *header, const u_char *content)
{
	struct ether_header *eptr;  /* net/ethernet.h */
	u_char *ptr; 
	sniff_ip *ip;

	//system();
  
	printf("Packet Num [%d]\n", cnt);
    cnt++;
	//cout << content << endl;
	eptr = (struct ether_header *) content;

	for(int i=0;i<header->caplen;i+=2)
	{
		printf("%02x", content[i]);
		printf("%02x ", content[i+1]);
	}

	printf("\n");

	int j;  
    
	ptr = eptr->ether_shost;  
    j = ETHER_ADDR_LEN;  
    printf("Source MAC: ");  
    do{  
        printf("%s%02x",(j == ETHER_ADDR_LEN) ? " " : ":",*ptr++);  
    }while(--j>0);  
    printf("\n"); 

    ptr = eptr->ether_dhost;
    j = ETHER_ADDR_LEN;  
    printf("Destination MAC: ");  
    do{  
        printf("%s%02x",(j == ETHER_ADDR_LEN) ? " " : ":",*ptr++);  
    }while(--j>0);  
    printf("\n");  
  
    printf("Ethernet type: ");

    switch(ntohs (eptr->ether_type))
    {
    	case ETHERTYPE_PUP:
    		printf("PUP\n");
    		break;
    	case ETHERTYPE_SPRITE:
    		printf("SPRITE\n");
    		break;
    	case ETHERTYPE_IP:
    		printf("IPv4\n");
    		break;
    	case ETHERTYPE_ARP:
    		printf("ARP\n");
    		break;
    	case ETHERTYPE_REVARP:
    		printf("REVARP\n");
    		break;
    	case ETHERTYPE_AT:
    		printf("AT\n");
    		break;
    	case ETHERTYPE_AARP:
    		printf("AARP\n");
    		break;
    	case ETHERTYPE_VLAN:
    		printf("VLAN\n");
    		break;
    	case ETHERTYPE_IPX:
    		printf("IPX\n");
    		break;
    	case ETHERTYPE_IPV6:
    		printf("IPv6\n");
    		break;
    	case ETHERTYPE_LOOPBACK:
    		printf("LOOPBACK\n");
    		break;
    }	

    ip = (struct sniff_ip*)(content + SIZE_ETHERNET);

    printf("Src IP %s\n", inet_ntoa(ip->ip_src));
    printf("Dst IP %s\n", inet_ntoa(ip->ip_dst));

    printf("Next Layer Protocol ");
    switch(ip->ip_p)
    {
    	case 0:
    		printf("HOPOPT\n");
    		break;
    	case 1:
    		printf("ICMP\n");
    		break;
    	case 4:
    		printf("IPv4\n");
    		break;
    	case 6:
    		printf("TCP\n");
    		break;
    	case 17:
    		printf("UDP\n");
    		break;
    	case 41:
    		printf("IPv6\n");
    		break;
    	case 47:
    		printf("GRE\n");
    		break;
    	case 58:
    		printf("IPv6-ICMP\n");
    		break;
    	default:
    		printf("%d\n", ip->ip_p);
    }

    printf("GRE:\n Next Layer Protocol:");

    int proto = (*(content+SIZE_ETHERNET+20+2) << 8 )+ *(content+SIZE_ETHERNET+20+2+1);

    switch(proto)
    {
    	case 0x0600:
    		printf("XNS\n");
    		break;
    	case 0x0800:
    		printf("IP\n");
    		break;
    	case 0x0806:
    		printf("ARP\n");
    		break;
    	case 0x6558:
    		printf("Transparent Ethernet Bridging\n");
    		break;
    	default:
    		break;
    }


    eptr = (struct ether_header *) (content+SIZE_ETHERNET+20+4);
    
    ptr = eptr->ether_shost; 
    j = ETHER_ADDR_LEN;  
    printf("Source MAC: ");  
    do{  
        printf("%s%02x",(j == ETHER_ADDR_LEN) ? " " : ":",*ptr++);  
    }while(--j>0);  
    printf("\n"); 

    ptr = eptr->ether_dhost;
    j = ETHER_ADDR_LEN;  
    printf("Destination MAC: ");  
    do{  
        printf("%s%02x",(j == ETHER_ADDR_LEN) ? " " : ":",*ptr++);  
    }while(--j>0);  
    printf("\n");

    printf("Ether type: %02x ", eptr->ether_type);

    switch(ntohs (eptr->ether_type))
    {
    	case ETHERTYPE_PUP:
    		printf("(PUP)\n");
    		break;
    	case ETHERTYPE_SPRITE:
    		printf("(SPRITE)\n");
    		break;
    	case ETHERTYPE_IP:
    		printf("(IPv4)\n");
    		break;
    	case ETHERTYPE_ARP:
    		printf("(ARP)\n");
    		break;
    	case ETHERTYPE_REVARP:
    		printf("(REVARP)\n");
    		break;
    	case ETHERTYPE_AT:
    		printf("(AT)\n");
    		break;
    	case ETHERTYPE_AARP:
    		printf("(AARP)\n");
    		break;
    	case ETHERTYPE_VLAN:
    		printf("(VLAN)\n");
    		break;
    	case ETHERTYPE_IPX:
    		printf("(IPX)\n");
    		break;
    	case ETHERTYPE_IPV6:
    		printf("(IPv6)\n");
    		break;
    	case ETHERTYPE_LOOPBACK:
    		printf("(LOOPBACK)\n");
    		break;
    }

    //printf("%s\n", filter);

    printf("\n\n");

    string src_ip = string(inet_ntoa(ip->ip_src));
 	string dst_ip = string(inet_ntoa(ip->ip_dst));

    if((src_ip == "140.115.0.1" || dst_ip == "140.115.0.1") && gre2_ex==0)
    {
    	system("ip link add GRE115 type gretap remote 140.115.0.1 local 140.113.0.1");
    	system("ip link set GRE115 up");
    	system("brctl addif br0 GRE115");
    	system("ip link set br0 up");

    	gre2_ex=1;
    }   

    if((src_ip == "140.114.0.1" || dst_ip == "140.114.0.1") && gre1_ex==0)
    {
    	system("ip link add GRE114 type gretap remote 140.114.0.1 local 140.113.0.1");
    	system("ip link set GRE114 up");
    	system("brctl addif br0 GRE114");
    	system("ip link set br0 up");
    	

    	gre1_ex=1;
    }
    


}

int main(int argc, char const *argv[])
{
	/* code */

	int i;  
    char *dev;   
    char errbuf[PCAP_ERRBUF_SIZE];  
    pcap_if_t *devices = NULL;
    struct ether_header *eptr;  /* net/ethernet.h */ 

    const u_char *packet;
    


    system("ip link add br0 type bridge");
    system("brctl addif br0 BRGr-eth0");
   
   	//get all devices
	if(-1 == pcap_findalldevs(&devices, errbuf)) {
    	fprintf(stderr, "pcap_findalldevs(): %s\n", errbuf);
    	exit(1);
	}//end if

	int num=0;
	for(pcap_if_t *d = devices ; d ; d = d->next) {
    	printf("%d Name: %s\n", num , d->name);
    	num++;
    }

    printf("Insert a number to select interface:\n");

    int id;

    scanf("%d",&id);

    num=0;

    for (pcap_if_t *d = devices ; d ; d = d->next)
    {
    	if(num==id)
    	{
    		handle = pcap_open_live(d->name, 65535, 1, 1, errbuf);
			printf("Start listening at $%s\n", d->name);
			if(!handle) {
    			fprintf(stderr, "pcap_open_live(): %s\n", errbuf);
    			exit(1);
			}
			break;
    	}
    	num++;
    }

    printf("Insert BPF filter expression:\n");

    cin.get();

    cin.getline(filter,200);

	printf("filter: %s\n", filter);

    struct bpf_program fcode;
    if(-1 == pcap_compile(handle, &fcode, filter, 1, PCAP_NETMASK_UNKNOWN)) {
        fprintf(stderr, "pcap_compile(): %s\n", pcap_geterr(handle));
        pcap_close(handle);
        exit(1);
    }

    pcap_setfilter(handle, &fcode);

    while(1) {
        struct pcap_pkthdr *header = NULL;
        const u_char *content = NULL;
        
        int ret = pcap_loop(handle, 1, pcap_callback1, NULL);
        
        if(gre1_ex==1&&gre1_ex_pre==0)
        {
        	gre1_ex_pre=1;
        	strcat(filter, " && not host 140.114.0.1");

    		if(-1 == pcap_compile(handle, &fcode, filter, 1, PCAP_NETMASK_UNKNOWN)) {
        		fprintf(stderr, "pcap_compile(): %s\n", pcap_geterr(handle));
        		pcap_close(handle);
       			exit(1);
    		}
    		pcap_setfilter(handle, &fcode);
        }
        if (gre2_ex==1&&gre2_ex_pre==0)
        {
        	gre2_ex_pre=1;
        	strcat(filter, " && not host 140.115.0.1");

    		if(-1 == pcap_compile(handle, &fcode, filter, 1, PCAP_NETMASK_UNKNOWN)) {
        		fprintf(stderr, "pcap_compile(): %s\n", pcap_geterr(handle));
        		pcap_close(handle);
       			exit(1);
    		}
    		pcap_setfilter(handle, &fcode);
        }
    }



    pcap_close(handle);

	pcap_freealldevs(devices);
  
	return 0;
}