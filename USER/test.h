
#ifndef __TEST_H
#define __TEST_H

#ifdef __cplusplus
 extern "C" {
#endif
static int _index_test=0;
	 #define TEST(x,y) (x)==(y)? printf("\r\n%d ͨ��",_index_test++):printf("\r\n%d ʧ��:%x,%x",_index_test++,(x),(y))
	 
#ifdef __cplusplus
 }
#endif
#endif
