#pragma once

#include <string>

struct MonsterConfig {
    std::string spriteFrameName;  // ����ͼƬ��Դ
    int health;                   // ����ֵ
    float speed;                  // �ƶ��ٶ�
    int damage;                   // ������
    int reward;                   // ��ɱ����
};

// �����������������,ֻ��������û�ж���
extern MonsterConfig pigConfig;
extern MonsterConfig wolfConfig;
