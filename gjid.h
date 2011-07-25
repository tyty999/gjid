// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#pragma once
#include "level.h"
#include "font.h"
#include "app.h"

//----------------------------------------------------------------------

class GJID : public CApp {
public:
    static GJID&	Instance (void);
protected:
			GJID (void);
    virtual void	OnIdle (void);
    virtual void	OnDraw (CGC& gc);
    virtual void	OnKey (key_t key);
private:
    enum EGameState {
	state_Title,
	state_Story,
	state_Game,
	state_Winner,
	state_Loser,
	state_Last
    };
private:
    void		GoToState (EGameState state);
    void		FillWithTile (CGC& gc, PicIndex tidx) const;
    void		DecodeBitmapWithTile (CGC& gc, const uint16_t* p, size_t n, PicIndex tidx) const;
    inline void		IntroScreen (CGC& gc);
    inline void		LoserScreen (CGC& gc);
    inline void		PrintStory (CGC& gc);
    inline void		WinnerScreen (CGC& gc);
    inline void		DrawLevel (CGC& gc);
    inline void		TitleKeys (key_t key);
    inline void		StoryKeys (key_t key);
    inline void		LevelKeys (key_t key);
    void		LoadData (const char* filename);
private:
    EGameState		m_State;
    uoff_t		m_StoryPage;
    uoff_t		m_Level;
    Level		m_CurLevel;
    CFont		m_Font;
    picvec_t		m_Pics;
    CPalette		m_Palette;
    vector<Level>	m_Levels;
};
