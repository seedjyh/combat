#define	TIMER_SEC			13012
#define TIME_REFRESH		10

#define TIMER_PLAYER_AMMO	13013
#define TIME_PLAYER_FIRE	300

#define MAX_PIXEL		3000

#define KEY_COLOR		0xF80F

class OBJECT
{
private:
public:
	int	iWide;		//图像宽度
	int	iHeight;	//图像高度
	int	iHP;		//物体生命值
	int	iType;		//物体类型：玩家0、玩家弹药2、敌人1、敌人弹药3
	int	cxPosition;	//物体中心的横坐标
	int	cyPosition;	//物体中心的纵坐标
	int	cxSpeed;	//物体的横向速度
	int	cySpeed;	//物体的纵向速度
	int	cxCrash;	//碰撞有效宽度
	int	cyCrash;	//碰撞有效高度


	HDC		hObjDC,		hOldObjDC;
	HBITMAP	hObjBitmap,	hOldObjBitmap;
	int		iPixel[MAX_PIXEL];
	OBJECT	() {}
	OBJECT	(int wide, int height, int hp, int type,
		int	xp, int yp, int xs, int ys,
		TCHAR *FileName)
	{
		this->iWide		= wide;
		this->iHeight	= height;
		this->iHP		= hp;
		this->iType		= type;
		this->cxPosition= xp;
		this->cyPosition= yp;
		this->cxSpeed	= xs;
		this->cySpeed	= ys;

		this->hObjBitmap= (HBITMAP)LoadImage(NULL, FileName, IMAGE_BITMAP, 0, 0,
			LR_LOADFROMFILE);
		GetBitmapBits(hObjBitmap, this->iWide * this->iHeight * 4, this->iPixel);
	}
protected:
};

class AMMO:public	OBJECT
{
public:
	AMMO	()	{}
	AMMO	(int wide, int height, int hp, int type,
		int	xp, int yp, int xs, int ys,
		TCHAR *FileName)
	{
		this->iWide		= wide;
		this->iHeight	= height;
		this->iHP		= hp;
		this->iType		= type;
		this->cxPosition= xp;
		this->cyPosition= yp;
		this->cxSpeed	= xs;
		this->cySpeed	= ys;
		this->hObjBitmap= (HBITMAP)LoadImage(NULL, FileName, IMAGE_BITMAP, 0, 0,
			LR_LOADFROMFILE);
		GetBitmapBits(hObjBitmap, this->iWide * this->iHeight * 4, this->iPixel);
	}
	INT	Move(int cxClient, int cyClient)
	{
		this->cxPosition	+= this->cxSpeed;
		this->cyPosition	+= this->cySpeed;
		return (	this->cxPosition < 0	||
				this->cxPosition > cxClient	||
				this->cyPosition < 0	||
				this->cyPosition > cyClient);
	}
	INT	Draw(HDC	hDC, HDC	hMemDC)
	{
		this->hObjDC	= CreateCompatibleDC(hDC);
		SetBitmapBits(hObjBitmap, this->iWide * this->iHeight * 4 , this->iPixel);
		hOldObjBitmap	= (HBITMAP)SelectObject(hObjDC, hObjBitmap);
		BitBlt(hMemDC,
			this->cxPosition-this->iWide/2,
			this->cyPosition-this->iHeight/2,
			this->iWide,
			this->iHeight,
			hObjDC,0,0,SRCCOPY);
		SelectObject(hObjDC, hOldObjBitmap);
		DeleteDC(hObjDC);
		return	0;
	}
protected:
private:
};

class PLAYER:public	OBJECT
{
private:
public:
	BOOL	bMoveUp;
	BOOL	bMoveDown;
	BOOL	bMoveLeft;
	BOOL	bMoveRight;
	BOOL	bFiring;

	PLAYER	()	{}
	PLAYER	(int wide, int height, int hp, int type,
		int	xp, int yp, int xs, int ys,
		TCHAR *FileName)
	{
		this->iWide		= wide;
		this->iHeight	= height;
		this->iHP		= hp;
		this->iType		= type;
		this->cxPosition= xp;
		this->cyPosition= yp;
		this->cxSpeed	= xs;
		this->cySpeed	= ys;
		this->bMoveUp	= FALSE;
		this->bMoveDown	= FALSE;
		this->bMoveLeft	= FALSE;
		this->bMoveRight= FALSE;
		this->bFiring	= FALSE;
		this->hObjBitmap= (HBITMAP)LoadImage(NULL, FileName, IMAGE_BITMAP, 0, 0,
			LR_LOADFROMFILE);
		GetBitmapBits(hObjBitmap, this->iWide * this->iHeight * 4, this->iPixel);
	}
	INT	Move(int cxClient, int cyClient)
	{
		if (this->bMoveLeft)
		{
			this->cxPosition	-= this->cxSpeed;
		}
		if (this->bMoveRight)
		{
			this->cxPosition	+= this->cxSpeed;
		}
		if (this->bMoveUp)
		{
			this->cyPosition	-= this->cySpeed;
		}
		if (this->bMoveDown)
		{
			this->cyPosition	+= this->cySpeed;
		}
		if (this->cxPosition < 0)
		{
			this->cxPosition = 0;
		}
		else if (this->cxPosition > cxClient)
		{
			this->cxPosition	= cxClient;
		}
		if (this->cyPosition < 0)
		{
			this->cyPosition = 0;
		}
		else if (this->cyPosition > cyClient)
		{
			this->cyPosition	= cyClient;
		}
		return	0;
	}
	INT	Draw(HDC	hDC, HDC	hMemDC)
	{
		this->hObjDC	= CreateCompatibleDC(hDC);
		SetBitmapBits(hObjBitmap, this->iWide * this->iHeight * 4 , this->iPixel);
		hOldObjBitmap	= (HBITMAP)SelectObject(hObjDC, hObjBitmap);
		BitBlt(hMemDC,
			this->cxPosition-this->iWide/2,
			this->cyPosition-this->iHeight/2,
			this->iWide,
			this->iHeight,
			hObjDC,0,0,SRCCOPY);
		SelectObject(hObjDC, hOldObjBitmap);
		DeleteDC(hObjDC);
		return	0;
	}
protected:
};