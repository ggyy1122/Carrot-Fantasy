#pragma once
#ifndef _music_H_
#define _music_H_
#include "cocos2d.h"
#include "AudioEngine.h"
USING_NS_CC;
class Music {
private:
	Music* instance;
	int music_play=1;  //音乐总开关，默认开
public:
	Music() {Music* instance = new Music;};
	~Music() { delete instance; };
	void Music::preloadSoundEffect(const std::string& music_file) {
		AudioEngine::preload(music_file);
	}
	int is_play() { return music_play; };  //返回音乐开关
	void set_music(int option) { music_play = option; };  //设置音乐开关
	void background_music() {   //背景音乐
		if(music_play)
			AudioEngine::play2d("CarrotGuardRes/Music/bgm.mp3", true, 0.5f);
	}
	void button_music(){        //按钮音乐
		if(music_play)
		AudioEngine::play2d("CarrotGuardRes/Music/button.mp3", false, 0.5f);
	}
	void page_music() {        //翻页音乐
		if (music_play)
			AudioEngine::play2d("CarrotGuardRes/Music/page.mp3", false, 0.2f);
	}
	void build_music() {        //建筑植物时音乐
		if (music_play)
			AudioEngine::play2d("CarrotGuardRes/Music/build.mp3", false, 1.0f);
	}
	void sell_music() {        //出售音乐
		if (music_play)
			AudioEngine::play2d("CarrotGuardRes/Music/sell.mp3", false, 1.0f);
	}
	void upgrade_music() {        //升级音乐
		if (music_play)
			AudioEngine::play2d("CarrotGuardRes/Music/upgrade.mp3", false, 1.0f);
	}
	//播放萝卜被吃音效
	void  carrotSound() {
		if (music_play)
			AudioEngine::play2d("CarrotGuardRes/Music/carrot.mp3", false, 1.0f);
	}
	//播放普通怪死亡音效
	void normalSound() {
		if (music_play)
			AudioEngine::play2d("CarrotGuardRes/Music/dead.mp3", false, 1.0f);
	}
	void countSound() {
		if (music_play)
			AudioEngine::play2d("CarrotGuardRes/Music/count.mp3", false, 1.0f);
	}
	void downSound() {
		if (music_play)
			AudioEngine::play2d("CarrotGuardRes/Music/down.mp3", false, 1.0f);
	}





	//植物，可添加
	void  sunSound() {
		if (music_play)
			AudioEngine::play2d("CarrotGuardRes/Music/sun.mp3", false, 1.0f);
	}
	//播放瓶子攻击音效
	void  bottleSound() {
		if (music_play)
			AudioEngine::play2d("CarrotGuardRes/Music/bottle.mp3", false, 1.0f);
	}
	//播放风扇攻击音效
	void  fanSound() {
		if (music_play)
			AudioEngine::play2d("CarrotGuardRes/Music/fan.mp3", false, 1.0f);
	}
};
#endif __music_H__