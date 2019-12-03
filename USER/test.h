
#ifndef __TEST_H
#define __TEST_H

#ifdef __cplusplus
 extern "C" {
#endif
static int _index_test=0;
	 #define TEST(x,y) (x)==(y)? printf("\r\n%d Í¨¹ý",_index_test++):printf("\r\n%d Ê§°Ü:%x,%x",_index_test++,(x),(y))
	 
#ifdef __cplusplus
 }
#endif
#endif
