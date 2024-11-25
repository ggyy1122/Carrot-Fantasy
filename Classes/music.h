#pragma once
#ifndef _music_H_
#define _music_H_
#include "cocos2d.h"
#include "AudioEngine.h"
USING_NS_CC;
class Music {
private:
	Music* instance;
	int music_play=1;  //�����ܿ��أ�Ĭ�Ͽ�
public:
	Music() {Music* instance = new Music;};
	~Music() { delete instance; };
	void Music::preloadSoundEffect(const std::string& music_file) {
		AudioEngine::preload(music_file);
	}
	int is_play() { return music_play; };  //�������ֿ���
	void set_music(int option) { music_play = option; };  //�������ֿ���
	void background_music() {   //��������
		if(music_play)
			AudioEngine::play2d("CarrotGuardRes/Music/bgm.mp3", true, 0.5f);
	}
	void button_music(){        //��ť����
		if(music_play)
		AudioEngine::play2d("CarrotGuardRes/Music/button.mp3", false, 0.5f);
	}
	void page_music() {        //��ҳ����
		if (music_play)
			AudioEngine::play2d("CarrotGuardRes/Music/page.mp3", false, 0.2f);
	}
	void build_music() {        //����ֲ��ʱ����
		if (music_play)
			AudioEngine::play2d("CarrotGuardRes/Music/build.mp3", false, 1.0f);
	}
	void sell_music() {        //��������
		if (music_play)
			AudioEngine::play2d("CarrotGuardRes/Music/sell.mp3", false, 1.0f);
	}
	void upgrade_music() {        //��������
		if (music_play)
			AudioEngine::play2d("CarrotGuardRes/Music/upgrade.mp3", false, 1.0f);
	}
	//�����ܲ�������Ч
	void  carrotSound() {
		if (music_play)
			AudioEngine::play2d("CarrotGuardRes/Music/carrot.mp3", false, 1.0f);
	}
	//������ͨ��������Ч
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





	//ֲ������
	void  sunSound() {
		if (music_play)
			AudioEngine::play2d("CarrotGuardRes/Music/sun.mp3", false, 1.0f);
	}
	//����ƿ�ӹ�����Ч
	void  bottleSound() {
		if (music_play)
			AudioEngine::play2d("CarrotGuardRes/Music/bottle.mp3", false, 1.0f);
	}
	//���ŷ��ȹ�����Ч
	void  fanSound() {
		if (music_play)
			AudioEngine::play2d("CarrotGuardRes/Music/fan.mp3", false, 1.0f);
	}
};
#endif __music_H__