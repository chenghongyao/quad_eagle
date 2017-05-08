#include "key.h"
#include "sys.h"


key_t mKeys[KEY_NUMBER];



key_status_t key_read_hw(key_name_t key)
{
	key_status_t res = KEY_NONE;
	switch(key)
	{
		case KEY_UP:
			if(keyUp == 0)res = KEY_PRESS;
			break;
		case KEY_DOWN:
			if(keyDown == 0)res = KEY_PRESS;
			break;
		default:
			break;
	}
	return res;
}


void key_setup()
{
	uint8_t i;
	for(i=0;i<KEY_NUMBER;i++)
	{
		mKeys[i].status = KEY_NONE;
		mKeys[i].flagPress = 0;
	}
}



//删除按键信号(trig)
void key_cancel(key_name_t key)
{
	mKeys[key].status = KEY_NONE;
}
	

void key_test()
{
	if(mKeys[KEY_UP].status == KEY_TRIG)
	{
		puts("trig->up");
		key_cancel(KEY_UP);
	}
		if(mKeys[KEY_DOWN].status == KEY_TRIG)
	{
		puts("trig->down");
		key_cancel(KEY_DOWN);
	}
	
	if(mKeys[KEY_LEFT].status == KEY_TRIG)
	{
		puts("trig->left");
		key_cancel(KEY_LEFT);
	}
	
	if(mKeys[KEY_RIGHT].status == KEY_TRIG)
	{
		puts("trig->right");
		key_cancel(KEY_RIGHT);
	}
	
	if(mKeys[KEY_ENT].status == KEY_TRIG)
	{
		puts("trig->ent");
		key_cancel(KEY_ENT);
	}
	
	if(mKeys[KEY_EXT].status == KEY_TRIG)
	{
		puts("trig->ext");
		key_cancel(KEY_EXT);
	}
}


void key_update()
{
	uint8_t i;
	for(i=0;i<KEY_NUMBER;i++)
	{
		if(key_read_hw((key_name_t)i) == KEY_PRESS)
		{
			if(mKeys[i].flagPress == 0)
			{
				mKeys[i].tStart = millis();
				mKeys[i].flagPress = 1;
			}
			else if(mKeys[i].status == KEY_NONE)
			{
				if((millis() - mKeys[i].tStart)>KEY_LONGPRESS_TIME)
				{
					mKeys[i].status = KEY_LONGPRESS;
				}
			}
		}
		else
		{
			if(mKeys[i].status == KEY_LONGPRESS)
			{
				mKeys[i].status = KEY_NONE;
			}
			else if(mKeys[i].flagPress == 1)
			{
				mKeys[i].status = KEY_TRIG;	
			}
			mKeys[i].flagPress = 0;
		}
	}
}

