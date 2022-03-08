
//ÎÞ·ûºÅ³ý
unsigned int __aeabi_uidiv(unsigned int y, unsigned int x)
{
	unsigned int i, t, tx;
	if (x == 0)
		return 0;
	i = 0;
	t = 1;
	tx = x;
	while (y >= x)
	{
		if (y >= tx)
		{
			y -= tx;
			i += t;
			if (y >= tx << 1)
			{

				t <<= 1;
				tx <<= 1;
			}
		}
		else
		{
			t >>= 1;
			tx >>= 1;
		}
		if (t <= 0)
			break;
	}
	return i;
}

//ÎÞ·ûºÅÓà
unsigned long long __aeabi_uidivmod(unsigned int y, unsigned int x)
{
	unsigned long long ret;
	unsigned int i, t, tx;
	if (x == 0)
		return y;
	i = 0;
	t = 1;
	tx = x;
	while (y >= x)
	{
		if (y >= tx)
		{
			y -= tx;
			i += t;
			if (y >= tx << 1)
			{

				t <<= 1;
				tx <<= 1;
			}
		}
		else
		{
			t >>= 1;
			tx >>= 1;
		}
		if (t <= 0)
			break;
	}
	ret=((long long)y)<<32 | i;
	return ret;
}
//ÓÐ·ûºÅ³ý
int __aeabi_idiv(int y, int x)
{
	int i, t, tx , flg ;
	if (x == 0)
		return 0;
	flg = 0;

	if (y < 0)
	{
		y = 0 - y;
		flg++;
	}
	if (x < 0)
	{
		x = 0 - x;
		flg--;
	}

	i = 0;
	t = 1;
	tx = x;
	while (y >= x)
	{
		if (y >= tx)
		{
			y -= tx;
			i += t;
			if (y >= tx << 1)
			{

				t <<= 1;
				tx <<= 1;
			}
		}
		else
		{
			t >>= 1;
			tx >>= 1;
		}
		if (t <= 0)
			break;
	}
	if (flg)
		return 0 - i;
	else
		return i;
}
//ÓÐ·ûºÅÓà
long long __aeabi_idivmod(int y, int x)
{
	long long ret;
	int i, t, tx, flg;
	if (x == 0)
		return y;
	flg = 0;

	if (y < 0)
	{
		y = 0 - y;
		flg++;
	}
	if (x < 0)
	{
		x = 0 - x;
		flg--;
	}

	i = 0;
	t = 1;
	tx = x;
	while (y >= x)
	{
		if (y >= tx)
		{
			y -= tx;
			i += t;
			if (y >= tx << 1)
			{

				t <<= 1;
				tx <<= 1;
			}
		}
		else
		{
			t >>= 1;
			tx >>= 1;
		}
		if (t <= 0)
			break;
	}
	if (flg)
	{
		ret=(0-(long long)y)<<32 | (0-i);
	}
	else
	{
		ret=((long long)y)<<32 | i;
	}
	return ret;
}
void __aeabi_memcpy(char *to , char *from , int count){
	char *to0 = to ;
	char *from0 = from ;
	char *to1 = to + 1 ;
	char *from1 = from + 1 ;
	int n = (count + 1) / 2 ;

	switch (count & 1){
	case 0: do{ *to1 = *from1 ; to1 += 2 ; from1 += 2 ;
	case 1:     *to0 = *from0 ; to0 += 2 ; from0 += 2 ;
			}while(--n>0);
	}
}
void __aeabi_memcpy4(char *to , char *from)
{
	__aeabi_memcpy(to,from,4);
}
void __aeabi_memcpy8(char *to , char *from)
{
	__aeabi_memcpy(to,from,8);
}

