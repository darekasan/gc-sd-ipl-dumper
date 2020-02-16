#include <gccore.h>
#include <fat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>

extern void __SYS_ReadROM(void *buf,u32 len,u32 offset);

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

void *Initialise();

int main(int argc, char **argv) {

	xfb = Initialise();
	FILE *dumpFile;
	char filename[] = "ipl.bin";
	u32 size = 0x200000;

	printf("\nGC SD IPL Dumper\n\n");
	u8 *ipl = memalign(32,size);
	__SYS_ReadROM(ipl,size,0);

	if(fatInitDefault()) {
		printf("Press Button A to dump IPL ROM(Start: Cancel).\n");
		while(1) {

			VIDEO_WaitVSync();
			PAD_ScanPads();

			int buttonsDown = PAD_ButtonsDown(0);
			
			if( buttonsDown & PAD_BUTTON_A ) {
				printf("just a moment...\n");
				dumpFile = fopen(filename,"wb");
				fwrite(ipl,sizeof(u8),size,dumpFile);

				printf("\nDone!\n");
				VIDEO_WaitVSync();
				sleep(5);

				exit(0);
			}

			if (buttonsDown & PAD_BUTTON_START) {
				exit(0);
			}
		}
	} else {
		printf("Error: FAT device could not be initialized!\n");
		sleep(5);
		exit(1);
	}

	return 0;
}

void * Initialise() {

	void *framebuffer;

	VIDEO_Init();
	PAD_Init();
	
	rmode = VIDEO_GetPreferredMode(NULL);

	framebuffer = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	console_init(framebuffer,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
	
	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(framebuffer);
	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();

	return framebuffer;

}
