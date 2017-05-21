#include "sbus.h"
#include "sys.h"


sbus_t mSbus;
void sbus_init(void)
{
	
	mSbus.channel.startbyte = 0xF0;
	mSbus.channel.ch1 = 0;
	mSbus.channel.ch2 = 0;
	mSbus.channel.ch3 = 0;
	mSbus.channel.ch4 = 0;
	mSbus.channel.ch5 = 0;
	mSbus.channel.ch6 = 0;
	mSbus.channel.ch7 = 0;
	mSbus.channel.ch8 = 0;
	mSbus.channel.ch9 = 0;
	mSbus.channel.ch10 = 0;
	mSbus.channel.ch11 = 0;
	mSbus.channel.ch12 = 0;
	mSbus.channel.ch13 = 0;
	mSbus.channel.ch14 = 0;
	mSbus.channel.ch15 = 0;
	mSbus.channel.ch16 = 0;
	mSbus.channel.flags.ch17 = 0;
	mSbus.channel.flags.ch18 = 0;
	mSbus.channel.flags.failsafe = 0;
	mSbus.channel.flags.framelost = 0;
	
}

