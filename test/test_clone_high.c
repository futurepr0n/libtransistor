#include<libtransistor/types.h>
#include<libtransistor/util.h>
#include<libtransistor/fs/inode.h>
#include<libtransistor/fs/squashfs.h>
#include<libtransistor/fs/fs.h>
#include<errno.h>
#include<stdio.h>
#include<dirent.h>
#include<fcntl.h>
#include<unistd.h>

#include"../lib/squashfs/squashfuse.h"

/******************************PS CODE *********/
// Defines
#define YES = 1
#define NO = 0
#define ON 1
#define OFF 0
#define MAX_NUM_ENEMIES 100
#define MAX_NUM_BULLETS 100
#define LEFT 0
#define RIGHT 1

// These are some variables for our use
int done = 0;
int bullets = 0;
int points = 0;
int hScore = 1000;


/*
 * These are for loading the Images
 */
extern unsigned char _binary_resources_image_png_start;
extern unsigned char _binary_resources_peter_png_start;
extern unsigned char _binary_resources_lockers_png_start;
extern unsigned char _binary_resources_abe_png_start;
extern unsigned char _binary_resources_fireball_png_start;

// Here we are going to define our structures for the objects
typedef struct{
	float x;
	float y;
	//int imgX;
	//int imgY;
	vita2d_texture *img;
	int isalive;
	int pctr;
}obj;

obj a_fireball[MAX_NUM_BULLETS];
obj enemy[MAX_NUM_ENEMIES];
obj a_player;
obj a_background1;
obj a_background2;

//Functions
void blitBackground(vita2d_texture *bg, float x, float y);
void blitBackgroundBW(vita2d_texture *bg, float x, float y);
void blitObj(obj object);
void blitEnemies();
void control(SceCtrlData p1, vitaWav *sound);
void printScore(vita2d_pgf *pgf, vita2d_pvf *pvf);
void loadPlayer();
void loadEnemies();
void loadCharacterData();
void loadFireball();
void shootFireball();
int checkCollision(obj blt);
void scrollBackground(vita2d_texture *bg1, vita2d_texture *bg2, vita2d_texture *bg3, float bg_x, float bg_y);

//a_player.x = 20.0f;
//a_player.y = 20.0f;

float bg_x = 0.0f;
float bg_y = 0.0f;
/***************************************************/


int main(int argc, char *argv[]) {
	result_t r;

	DIR *d = opendir("/");
	if(d == NULL) {
		printf("failed to open directory: %d\n", errno);
		return 1;
	}

	struct dirent *dent;
	while((dent = readdir(d)) != NULL) {
		printf("dent: %.*s\n", dent->d_namlen, dent->d_name);
	}
	closedir(d);
	
	printf("trn_fs_open:\n");
	int fd;
	if((r = trn_fs_open(&fd, "/foo.bar", 0)) != RESULT_OK) {
		printf("failed to open: 0x%x\n", r);
		return 1;
	}
	printf("opened: %d\n", fd);

	char buf[256];
	ssize_t rd = read(fd, buf, sizeof(buf));
	if(rd < 0) {
		printf("err\n");
		return 1;
	}
	printf("read: %s\n", buf);

	FILE *f = fopen("/./testdir/../.././testdir/./testfile", "rb");
	if(f == NULL) {
		printf("fopen failure: %d\n", errno);
		return 1;
	}
	rd = fread(buf, sizeof(*buf), sizeof(buf)/sizeof(*buf), f);
	if(rd == 0) {
		printf("fread failure\n");
		return 1;
	}
	printf("fread: %d: %.*s\n", rd, rd, buf);
	
	return 0;
}


void blitObj(obj object){
	vita2d_draw_texture(object.img, object.x, object.y);
}

void blitBackground(vita2d_texture *bg, float x, float y){
	float newX = x+512;
	vita2d_draw_texture(bg, newX, y);
	vita2d_draw_texture(bg, newX + 512, y);

}

void blitBackgroundBW(vita2d_texture *bg, float x, float y){
	float newX = x-512;
	vita2d_draw_texture(bg, newX, y);
	vita2d_draw_texture(bg, newX - 512, y);

}

int scrollBackground(vita2d_texture *bg1, vita2d_texture *bg2, vita2d_texture *bg3, float bg_x, float bg_y){

		if(bg_x < 0){
    	      		blitBackground(bg1, bg_x, bg_y);
 	   		if(bg_x<-512){
     	   			bg_x=0;
     	  }
    	}

		if(bg_x > 0){
			blitBackgroundBW(bg1, bg_x, bg_y);
			if(bg_x>+512){
				bg_x=0;
			}
		}
		vita2d_draw_texture(bg1, bg_x, bg_y);
		vita2d_draw_texture(bg2, bg_x + 512, bg_y);
		vita2d_draw_texture(bg3, bg_x + 1024, bg_y);
	
		return bg_x, bg_y;
}

void loadCharacterData(){
	loadPlayer();
	loadEnemies();
	loadFireball();
}

void control(SceCtrlData p1, vitaWav *sound){

	if (p1.buttons & SCE_CTRL_START){
		//break;
	}

	if(p1.buttons & SCE_CTRL_SQUARE){
		// fireball_x = a_player.x + 10;
		// fireball_y = a_player.y;
		vitaWavPlay(sound);
		shootFireball();
	}

	if (p1.buttons & SCE_CTRL_RIGHT && a_player.x <= 800){
	//	peter_x += 10.0f;
		if(a_player.x <= 150){
			a_player.x += 10.0f;
		} else {
			bg_x -= 10.0f;
		}
	}

	if (p1.buttons & SCE_CTRL_LEFT && a_player.x >= 20){
	//	peter_x -= 10.0f;
		if(a_player.x >= 60){
			a_player.x -= 10.0f;
		}else{
			bg_x += 10.0f;
		}

	}

	if (p1.buttons & SCE_CTRL_UP && a_player.y >= 20){
		a_player.y -= 10.0f;
	}

	if (p1.buttons & SCE_CTRL_DOWN && a_player.y <= 400){
		a_player.y += 10.0f;

	}


	if (p1.lx >= 130 && a_player.x <= 800) {
	//	peter_x += 10.0f;
		if(a_player.x <= 150){
			a_player.x += 10.0f;
		} else {
			bg_x -= 10.0f;
		}
	}

	if (p1.lx <= 120 && a_player.x >= 20 ){
	//	peter_x -= 10.0f;
		if(a_player.x >= 60){
			a_player.x -= 10.0f;
		}else{
			bg_x += 10.0f;
		}
	}
	if (p1.ly >= 140 && a_player.y <= 400){
		a_player.y += 10.0f;

	}

	if (p1.ly <= 120 && a_player.y >= 20){
		a_player.y -= 10.0f;

	}

}

void loadPlayer(){
	a_player.x = 64.0f;
	a_player.y = 128.0f;
	//player.imgX = 46;
	//player.imgY = 24;
	a_player.img = vita2d_load_PNG_buffer(&_binary_resources_abe_png_start);
	if(!a_player.img){
		printf("Player image failed to load...");
	}
	a_player.isalive = 1;
	//blitObj(player);
	//return player;
}

void loadEnemies(){
	int i;
	vita2d_texture *peter = vita2d_load_PNG_buffer(&_binary_resources_peter_png_start);
	for(i = 0; i < MAX_NUM_ENEMIES; i++){
		enemy[i].x = 960 + ((int)rand()%10000);
		enemy[i].y = 1 + ((int)rand()%540 - 34);
		enemy[i].img = peter;
		if(!enemy[i].img){
			printf("Enemy image failed to load...");
		}
		enemy[i].isalive = 1;
		enemy[i].pctr = 0;
		//blitObj(enemy[i]);
	}
	//return enemy[i];
}

void blitEnemies(){
	int enemy_ctr;
	for(enemy_ctr = 0; enemy_ctr < MAX_NUM_ENEMIES; enemy_ctr++)
		{
			if(enemy[enemy_ctr].isalive == 1){
				float mtpl = ((int)rand()%7) * 2.1;

	     		enemy[enemy_ctr].x = enemy[enemy_ctr].x - mtpl;
				if(enemy[enemy_ctr].x < 0){
					enemy[enemy_ctr].isalive = 0;
				}
	     		blitObj(enemy[enemy_ctr]);
	     	} else {
	     		//free up memory.. do not blit to screen
	     		enemy[enemy_ctr].x = 960;
	     	}
		}
}


void loadFireball(){
	int i;
	vita2d_texture *temp = vita2d_load_PNG_buffer(&_binary_resources_fireball_png_start);
	for(i = 0; i < MAX_NUM_BULLETS; i++){
		a_fireball[i].x = 0;
		a_fireball[i].y = 0;
		a_fireball[i].img = temp;
		if(!a_fireball[i].img){
			printf("a_fireball image failed to load...");
		}
		a_fireball[i].isalive = 0;
		a_fireball[i].pctr = 0;
	}
}

void shootFireball(){
	if(bullets < MAX_NUM_BULLETS && a_fireball[bullets].isalive == 0){
		a_fireball[bullets].isalive = 1;
		a_fireball[bullets].x = a_player.x + 42;
		a_fireball[bullets].y = a_player.y + (a_player.y / 2);
	}

	bullets++;
	if(bullets > MAX_NUM_BULLETS){
		bullets = 0;
	}
}

void printScore(vita2d_pgf *pgf, vita2d_pvf *pvf){
	vita2d_pgf_draw_text(pgf, 700, 430, RGBA8(0,255,0,255), 1.0f, "Clone High");
	vita2d_pgf_draw_text(pgf, 500, 430, RGBA8(0,255,0,255), 1.0f, "vitaWav fireball");
	vita2d_pvf_draw_text(pvf, 700, 480, RGBA8(0,255,0,255), 1.0f, "by futurepr0n!");
}

int checkCollision(obj blt){
	int c_ctr;
	if(blt.isalive == 1){
		//blt.x = blt.x + (8 * 1.5F);
		for(c_ctr = 0; c_ctr < MAX_NUM_ENEMIES; c_ctr++){
			if((blt.x >= enemy[c_ctr].x && blt.y < enemy[c_ctr].y + 20) && (blt.x < enemy[c_ctr].x + 42 && blt.y >= enemy[c_ctr].y)){
				enemy[c_ctr].isalive = 0;
				points = points + 10;
				blt.isalive = 0;
			}
		}
	}
	return(blt.isalive);
}





