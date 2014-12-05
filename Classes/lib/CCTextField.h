//
//  CCTextField.h
//  CCTextField
//
//  Created by Mit on 12/12/13.
//
//

#ifndef __CCTextField__CCTextField__
#define __CCTextField__CCTextField__
#include "cocos2d.h"

#define TEXTFIELD_KEYBOARD_WILL_SHOW_NOTIFICATION      "textfieldkeyboardWillShow"
#define TEXTFIELD_KEYBOARD_WILL_HIDDEN_NOTIFICATION    "textfieldkeyboardWillHide"

USING_NS_CC;
#include <iostream>
class CCTextField:public CCTextFieldTTF, public CCTextFieldDelegate, public CCTouchDelegate
{
    
public:
public:
     CCTextField();
    ~CCTextField();
    
    // static
    static CCTextField* textFieldWithPlaceHolder(const char *placeholder, const char *fontName, float fontSize);
    float getTextDimensions(std::string text);
    
    // CCLayer
    void onEnter();
    void onExit();
    
    // 初始化光标可以设置光标的高度
    void initCursorSprite(int nHeight);
    
    // CCTextFieldDelegate
    virtual bool onTextFieldAttachWithIME(CCTextFieldTTF *pSender);
    virtual bool onTextFieldDetachWithIME(CCTextFieldTTF * pSender);
    virtual bool onTextFieldInsertText(CCTextFieldTTF * pSender, const char * text, int nLen);
    virtual bool onTextFieldDeleteBackward(CCTextFieldTTF * pSender, const char * delText, int nLen);
    
    virtual void keyboardWillShow(cocos2d::CCIMEKeyboardNotificationInfo& info);
    virtual void keyboardWillHide(cocos2d::CCIMEKeyboardNotificationInfo& info);
    
 
    bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    
    // 判断是否点击在TextField处
    bool isInTextField(CCTouch *pTouch);
    
    // 得到TextField矩形
    CCRect getRect();
    
    virtual bool attachWithIME();
    virtual bool detachWithIME();
    

    CC_SYNTHESIZE(CCIMEKeyboardNotificationInfo,keyBoardInfo,keyBoardInfo);
    
private:
    // 点击开始位置
    CCPoint m_beginPos;
    
    // 光标精灵
    CCSprite *m_pCursorSprite;
    
    // 光标动画
    CCAction *m_pCursorAction;
    
    // 光标坐标
    CCPoint m_cursorPos;
    
};
#endif /* defined(__CCTextField__CCTextField__) */
