//
//  CCTextField.cpp
//  CCTextField
//
//  Created by Mit on 12/12/13.
//
//

#include "CCTextField.h"

//const static float DELTA = 0.5f;

CCTextField::CCTextField()
{
    CCTextFieldTTF();
    m_pCursorSprite = NULL;
    m_pCursorAction = NULL;
}

CCTextField::~CCTextField()
{
    this->detachWithIME();
    
}

float CCTextField::getTextDimensions(std::string text)
{
    static char bulletString[] = {(char)0xe2, (char)0x80, (char)0xa2, (char)0x00};
    std::string displayText=text;
    
    if (m_bSecureTextEntry) {
        displayText="";
        int length =strlen(text.c_str());
        while (length)
        {
            displayText.append(bulletString);
            --length;
        }
    }
    
    CCLabelTTF *label=CCLabelTTF::create(displayText.c_str(),this->getFontName(), this->getFontSize());
    
    float width=label->getContentSize().width;
    
    if (width>this->getDimensions().width) {
        width=this->getDimensions().width;
    }
    
    return width;
}


void CCTextField::onEnter()
{
    CCTextFieldTTF::onEnter();
    
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,-1, false);
    m_pCursorSprite->setPosition(CCPoint(0,this->getContentSize().height/2));
    this->setDelegate(this);
}

CCTextField * CCTextField::textFieldWithPlaceHolder(const char *placeholder, const char *fontName, float fontSize)
{
    CCTextField *pRet = new CCTextField();
    
    if(pRet && pRet->initWithString("", fontName, fontSize))
    {
        pRet->autorelease();
        if (placeholder)
        {
            pRet->setPlaceHolder(placeholder);
        }
        pRet->setVerticalAlignment(kCCVerticalTextAlignmentCenter);
        pRet->initCursorSprite(fontSize);
        
        return pRet;
    }
    
    CC_SAFE_DELETE(pRet);
    
    return NULL;
}

void CCTextField::initCursorSprite(int nHeight)
{
    // 初始化光标
    int column = 4;
    int pixels[nHeight][column];
    for (int i=0; i<nHeight; ++i) {
        for (int j=0; j<column; ++j) {
            pixels[i][j] = 0xffffffff;
        }
    }
    
    CCTexture2D *texture = new CCTexture2D();
    texture->initWithData(pixels, kCCTexture2DPixelFormat_RGB888, 1, 1, CCSizeMake(column, nHeight));
    
    m_pCursorSprite = CCSprite::createWithTexture(texture);
    CCSize winSize = getContentSize();
    
    m_cursorPos = ccp(0, winSize.height/2);
    m_pCursorSprite->setAnchorPoint(CCPoint(0.5, 0.5));
    m_pCursorSprite->setPosition(m_cursorPos);
    this->addChild(m_pCursorSprite);
    
    m_pCursorAction=CCRepeatForever::create(CCSequence::create(CCFadeOut::create(0.5f), CCFadeIn::create(0.5f), NULL));
    
    m_pCursorSprite->runAction(m_pCursorAction);
    m_pCursorSprite->setVisible(false);
    m_pInputText = new std::string();
}



CCRect CCTextField::getRect()
{
    CCSize size = getContentSize();
//    CCLog("width=%f,height=%f",size.width,size.height);
  
    if (size.width==0&&size.height==0) {
        size=this->getDimensions();
    }
    
    CCPoint anchorPoint=this->getAnchorPoint();
    
    return  CCRectMake(-size.width*anchorPoint.x, -size.height*anchorPoint.y,size.width, size.height);
}


bool CCTextField::isInTextField(cocos2d::CCTouch *pTouch)
{
//    CCPoint point=this->convertTouchToNodeSpaceAR(pTouch);
//    CCLog("point.x=%f,point.y=%f",point.x,point.y);
    CCRect rect=this->getRect();
//    CCLog("%f,%f",rect.size.width,rect.size.height);
    return rect.containsPoint(convertTouchToNodeSpaceAR(pTouch));
}


bool CCTextField::ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
    m_beginPos = pTouch->getLocationInView();
    return true;
}



void CCTextField::ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
    //切换输入状态
    isInTextField(pTouch) ? attachWithIME() : detachWithIME();
    
}

bool CCTextField::onTextFieldAttachWithIME(cocos2d::CCTextFieldTTF *pSender)
{
    if (m_pInputText->empty()) {
        return false;
    }
    
    m_pCursorSprite->cocos2d::CCNode::setPosition(getTextDimensions(m_pInputText->c_str()),this->getContentSize().height/2);
    return false;
}

bool CCTextField::onTextFieldInsertText(cocos2d::CCTextFieldTTF *pSender, const char *text, int nLen)
{
    
    if(text[0] == '\n')
    {
        m_pCursorSprite->setVisible(false);
		return false;
    }
    
    
    std::string tempString=*m_pInputText;
    float textDimension=this->getTextDimensions(tempString);
    float addWidth=this->getTextDimensions(CCStringMake(text)->m_sString);
    
    CCLog("input text::%s  inputText=%s",this->getString(),text);
    if (textDimension+addWidth>=this->getDimensions().width) {
        
        m_pCursorSprite->cocos2d::CCNode::setPosition(textDimension,this->getContentSize().height/2);
        setString(tempString.c_str());
        return true;
    }
    
    m_pCursorSprite->cocos2d::CCNode::setPosition(textDimension+addWidth,this->getContentSize().height/2);
    return false;
}

bool CCTextField::onTextFieldDeleteBackward(cocos2d::CCTextFieldTTF *pSender, const char *delText, int nLen)
{
    float decWidth=this->getTextDimensions(CCStringMake(delText)->m_sString);
    
    m_pCursorSprite->cocos2d::CCNode::setPosition(this->getTextDimensions(m_pInputText->c_str())-decWidth,this->getContentSize().height/2);
    
    return false;
}

bool CCTextField::onTextFieldDetachWithIME(cocos2d::CCTextFieldTTF *pSender)
{
    return false;
}

void CCTextField::keyboardWillShow(cocos2d::CCIMEKeyboardNotificationInfo& info)
{

    this->setkeyBoardInfo(info);
    CCNotificationCenter::sharedNotificationCenter()->postNotification(TEXTFIELD_KEYBOARD_WILL_SHOW_NOTIFICATION,this);
  
}




void CCTextField::keyboardWillHide(cocos2d::CCIMEKeyboardNotificationInfo& info)
{
//    CCLog("keyboardDidHidden");
    this->setkeyBoardInfo(info);
    CCNotificationCenter::sharedNotificationCenter()->postNotification(TEXTFIELD_KEYBOARD_WILL_HIDDEN_NOTIFICATION,this);
    
}


bool CCTextField::attachWithIME()
{
    m_pCursorSprite->setVisible(true);
    m_pCursorSprite->setPosition(ccp(0,this->boundingBox().size.height/2));
    CCTextFieldTTF::attachWithIME();
    return false;
}

bool CCTextField::detachWithIME()
{
    m_pCursorSprite->setVisible(false);
    CCTextFieldTTF::detachWithIME();
    return false;
}



void CCTextField::onExit()
{
    CCTextFieldTTF::onExit();
    CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
    
}