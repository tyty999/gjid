// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#pragma once
#include "level.h"
#include "font.h"
#include "app.h"

#define DATAFILE	"data/gjid.dat"

//----------------------------------------------------------------------

class GJID : public CApplication {
public:
    static GJID&	Instance (void);
    virtual void	ProcessArguments (argc_t argc, argv_t argv);
protected:
			GJID (void);
    virtual void	OnCreate (void);
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
	state_Editor,
	state_Last
    };
private:
    void		GoToState (EGameState state);
    void		FillWithTile (CGC& gc, PicIndex tidx) const;
    void		DecodeBitmapWithTile (CGC& gc, const uint16_t* p, size_t n, PicIndex tidx) const;
    void		IntroScreen (CGC& gc);
    void		LoserScreen (CGC& gc);
    void		PrintStory (CGC& gc);
    void		WinnerScreen (CGC& gc);
    void		DrawLevel (CGC& gc);
    void		DrawEditor (CGC& gc);
    void		TitleKeys (key_t key);
    void		StoryKeys (key_t key);
    void		LevelKeys (key_t key);
    void		WinnerKeys (key_t key);
    void		LoserKeys (key_t key);
    void		EditorKeys (key_t key);
    void		LoadData (const char* filename);
    void		SaveData (const char* filename) const;
private:
    EGameState		m_State;
    string		m_EditedPackage;
    uoff_t		m_StoryPage;
    uoff_t		m_Level;
    Level		m_CurLevel;
    PicIndex		m_SelectedPic;
    Point		m_SelectedTile;
    CFont		m_Font;
    picvec_t		m_Pics;
    CPalette		m_Palette;
    vector<Level>	m_Levels;
};
