#include <stdio.h>  
#include <string.h> 
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <net/if_dl.h>
#include <sys/sysctl.h>
#include <sys/stat.h>

#include "utils.h"
#include "iggcrypt.h"
#include "gate1.h"
#include "gate2.h"
#include "gate3.h"
#include "gg_daemon.h"
#include "gg_trigger.h"

#define IGG_LOADER_731_CSUM 0x7276EFD4

int check_installed_igg_version(void)
{
	int ret;
	uint32_t checksum;
	struct stat st;

	if(0 != stat("/Applications/iGameGuardian.app", &st)){
		printf("Erro, install iGameGuardian first\n");
		return -1;
	}
	ret = igg_checksum("/Applications/iGameGuardian.app/loader", &checksum);
	if(checksum != IGG_LOADER_731_CSUM){
		printf("Erro, the iGameGuardian your installed is not 7.3.1\n");
		return -1;
	}
	return 0;
}

uint8_t *get_wifi_mac(void)
{
	int mib[6];
	size_t len;
	uint8_t *mac, *ptr, *buf;
	struct if_msghdr *ifm;
	struct sockaddr_dl *sdl;

	mib[0] = CTL_NET;
	mib[1] = AF_ROUTE;
	mib[2] = 0;
	mib[3] = AF_LINK;
	mib[4] = NET_RT_IFLIST;
	if ((mib[5] = if_nametoindex("en0")) == 0) {
		printf("Error: if_nametoindex error/n");
		return NULL;
	}
	if (sysctl(mib, 6, NULL, &len, NULL, 0) < 0) {
		printf("Error: sysctl, take 1/n");
		return NULL;
	}
	if ((buf = (uint8_t *)malloc(len)) == NULL) {
		printf("Could not allocate memory. error!/n");
		return NULL;
	}
	if (sysctl(mib, 6, buf, &len, NULL, 0) < 0) {
		printf("Error: sysctl, take 2");
		free(buf);
		return NULL;
	}
	mac = (uint8_t *)malloc(6);
	ifm = (struct if_msghdr *)buf;
	sdl = (struct sockaddr_dl *)(ifm + 1);
	ptr = (unsigned char *)LLADDR(sdl);
	memcpy(mac, ptr, 6);
	free(buf);
	return mac;
}

int check_nvram_setting(void)
{
	return system("nvram wifiaddr");
}

int main(int argc, char **argv)  
{
	uint32_t installed_seconds;
	uint8_t *wifimac;
	struct stat st;
	int ret;

	printf("iGameGuardian7.3.1 local activator, by jerryxjtu\n\n");

	if(check_installed_igg_version()<0)
		return -1;
	printf("+ check igg version\n");

	if(0 == check_nvram_setting()){
		printf("you have setting wifi mac addr in nvram, which is not allowed, delete it first by command:\nnvram -d wifiaddr\n");
		return -1;
	}
	printf("+ check wifi mac setting in nvram\n");

	wifimac = get_wifi_mac();
	if(NULL == wifimac){
		printf("can't get wifi mac...\nexit\n");
		return -1;
	}
	printf("+ get wifi mac\n");

	ret = stat("/Applications/iGameGuardian.app/loader", &st);
	if(0 != ret){
		printf("can't stat loader...\nexit\n");
		return -1;
	}
	installed_seconds = st.st_ctimespec.tv_sec;
	printf("+ get installed timestame\n");

	// install gg_trigger
	save_file("/usr/libexec/gg_trigger", (void *)gg_trigger_payload, gg_trigger_payload_length-1);
	system("chown root:wheel /usr/libexec/gg_trigger");
	system("chmod 6755 /usr/libexec/gg_trigger");
	printf("+ install /usr/libexec/gg_trigger\n");

	// install com.apple.gg.daemon.plist
	save_file("/Library/LaunchDaemons/com.apple.gg.daemon.plist", (void *)gg_daemon_payload, gg_daemon_payload_length-1);
	save_file("/System/Library/LaunchDaemons/com.apple.gg.daemon.plist", (void *)gg_daemon_payload, gg_daemon_payload_length-1);
//	system("chown root:wheel /Library/LaunchDaemons/com.apple.gg.daemon.plist1");
//	system("chmod 644 /Library/LaunchDaemons/com.apple.gg.daemon.plist1");
//	system("chown root:wheel /System/Library/LaunchDaemons/com.apple.gg.daemon.plist1");
//	system("chmod 644 /System/Library/LaunchDaemons/com.apple.gg.daemon.plist1");
	printf("+ install /Library/LaunchDaemons/com.apple.gg.daemon.plist\n");

	// install gate1 2 3
	save_file("tmp.bin", (void *)gate1_payload, gate1_payload_length);
	gate1_crypt_op("tmp.bin", "/System/Library/LaunchDaemons/com.apple.gg.gate1.plist", wifimac, DO_ENCRYPT);
	save_file("tmp.bin", (void *)gate2_payload, gate2_payload_length);
	gate2_crypt_op("tmp.bin", "/System/Library/LaunchDaemons/com.apple.gg.gate2.plist", wifimac, installed_seconds, DO_ENCRYPT);
	save_file("tmp.bin", (void *)gate3_payload, gate3_payload_length);
	gate3_crypt_op("tmp.bin", "/System/Library/LaunchDaemons/com.apple.gg.gate3.plist", wifimac, installed_seconds, DO_ENCRYPT);
	unlink("tmp.bin");
	free(wifimac);
	printf("+ install gate1/2/3\n");

	// update ... /var/mobile/Library/com.apple.gg.record4.plist
	printf("+ update /var/mobile/Library/com.apple.gg.record4.plist\n");
	installed_seconds = 1;
	save_file("/var/mobile/Library/com.apple.gg.record4.plist", &installed_seconds, 4);

	// update ... /Applications/iGameGuardian.app/timestamp
	installed_seconds = time(0) + 3600*24*365*5; // five years later.
	save_file("/Applications/iGameGuardian.app/timestamp", &installed_seconds, 4);
	printf("+ update /Applications/iGameGuardian.app/timestamp\n");
	printf("+ done!\n\n");
	return 0;
}

