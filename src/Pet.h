// Pet.h: interface for the CPet class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _NEKO_PET_H_
#define _NEKO_PET_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "framework.h"

class CPet {

public:
	//constructor/destructor
	CPet();
	virtual ~CPet();

	//public configuration functions
	virtual void SetImages( HICON* hIconTable, int nIcons );
	virtual void SetScale( float fScale );

	//public interface
	virtual void SetImageAndMoveTo( int nImage, int nNewX, int nNewY );
	virtual void MoveTo( int nNewX, int nNewY );
	virtual void SetImage( int nImage );

    virtual void DrawOnTarget( int x, int y, HICON hIcon ) {}; //no default implementation

	//data member access
	inline RECT   GetBoundsRect()  { return m_rcBounds; };
	inline POINT& GetPosition()	   { return m_ptPosition; };
	inline POINT& GetOldPosition() { return m_ptOldPosition; };
    inline SIZE   GetSize()        { return m_sizeImage; };

protected:

	//pure virtual functions
	virtual void Erase() = 0;
	virtual void Draw( int nImage ) = 0;

	//image destruction
	virtual void DestroyImages();

	//icon information
	HICON* m_hIcons;
	int m_nIcons;
	int m_nLastIcon;

	//position & size information
	POINT m_ptPosition;
	POINT m_ptOldPosition;
	SIZE  m_sizeImage;     // scaled (display) size
	SIZE  m_sizeOriginal;  // unscaled icon mask size — kept verbatim from the
	                       // mask bitmap so region-building doesn't have to
	                       // reconstruct it via lossy int(scaled/m_fScale).
	float m_fScale;

	//bounding box information
	RECT m_rcBounds;
};

#endif // _NEKO_PET_H_
