#include "MouseRuleConfig.hpp"
#include <QXmlStreamWriter>

MouseRuleConfig::MouseRuleConfig(MouseRuleObserver *observer)
: mObserver(observer)
, mMouseRules()
, mMaxRules(10)
, mIsInsideMouseRuleConfig(false)
, mIsInsideMouseRule(false)
{
}


const MouseRules &MouseRuleConfig::rules() const
{
    return mMouseRules;
}


void MouseRuleConfig::addRule(const MouseRule *origRule)
{
    MouseRule *rule = 0;
    if (mMouseRules.size() < mMaxRules)
    {
        rule = origRule ? new MouseRule(0,
                                        origRule->position(), origRule->positionMode(),
                                        origRule->interval(), origRule->intervalMode(),
                                        origRule->action(), origRule->actionMode())
                        : new MouseRule();
        rule->setPredecessor(mMouseRules.empty() ? 0 : mMouseRules.back());
        mMouseRules.append(rule);
        update();
    }
    if (mObserver && rule)
    {
        mObserver->ruleAdded(*rule);
    }
}


void MouseRuleConfig::removeRule(MouseRule *rule)
{
    if (mMouseRules.size() > 1)
    {
        if (rule)
        {
            int idx = mMouseRules.indexOf(rule);
            Q_ASSERT(idx > -1);
            if (idx + 1 < mMouseRules.size())
            {
                mMouseRules[idx + 1]->setPredecessor(idx > 0 ? mMouseRules[idx - 1] : 0);
            }
            mMouseRules.removeAt(idx);
            update();
            if (mObserver)
            {
                mObserver->ruleRemoved(*rule);
            }
        }
    }
}


void MouseRuleConfig::invoke(MouseRobot &robot)
{
    for (QList<MouseRule*>::iterator i = mMouseRules.begin(); i != mMouseRules.end(); ++i)
    {
        (*i)->invoke(robot);
    }
}


void MouseRuleConfig::load(const QString &fileName)
{
    for (MouseRules::iterator i = mMouseRules.begin(); i != mMouseRules.end(); ++i)
    {
        mObserver->ruleRemoved(**i);
    }
    mMouseRules.clear();
    mIsInsideMouseRuleConfig = false;
    mIsInsideMouseRule = false;

    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    QXmlInputSource *source = new QXmlInputSource(&file);
    QXmlSimpleReader xmlReader;
    xmlReader.setDeclHandler(this);
    xmlReader.setDTDHandler(this);
    xmlReader.setEntityResolver(this);
    xmlReader.setLexicalHandler(this);
    xmlReader.setContentHandler(this);
    xmlReader.setErrorHandler(this);
    if (!xmlReader.parse(source))
    {
        qDebug("%s", errorString().toLatin1().data());
    }
    if (mMouseRules.empty())
    {
        addRule();
    }
    file.close();
}


void MouseRuleConfig::save(const QString &fileName)
{
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);
    QXmlStreamWriter stream(&file);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();
    stream.writeStartElement("MouseRuleConfig");
    for (MouseRules::iterator i = mMouseRules.begin(); i != mMouseRules.end(); ++i)
    {
        stream.writeStartElement("MouseRule");
        stream.writeAttribute("x", QString::number((*i)->position().x()));
        stream.writeAttribute("y", QString::number((*i)->position().y()));
        switch ((*i)->positionMode())
        {
            case CurrentPosition: stream.writeAttribute("posMode", "cur"); break;
            case AbsolutePosition: stream.writeAttribute("posMode", "abs"); break;
            case RelativePosition: stream.writeAttribute("posMode", "rel"); break;
        }
        stream.writeAttribute("interval", QString::number((*i)->interval()));
        switch ((*i)->intervalMode())
        {
            case MillisecondsInterval: stream.writeAttribute("intervalMode", "ms"); break;
            case SecondsInterval: stream.writeAttribute("intervalMode", "s"); break;
            case MinutesInterval: stream.writeAttribute("intervalMode", "m"); break;
            case HoursInterval: stream.writeAttribute("intervalMode", "h"); break;
        }
        stream.writeAttribute("action", QString::number((*i)->action()));
        switch ((*i)->actionMode())
        {
            case CurrentPosition: stream.writeAttribute("actionMode", "button"); break;
            case AbsolutePosition: stream.writeAttribute("actionMode", "key"); break;
            case RelativePosition: stream.writeAttribute("actionMode", "none"); break;
        }
        stream.writeEndElement();
    }
    stream.writeEndElement();
    stream.writeEndDocument();
    file.close();
}


void MouseRuleConfig::update()
{
    for (MouseRules::iterator i = mMouseRules.begin(); i != mMouseRules.end(); ++i)
    {
        if (i == mMouseRules.begin())
        {
            (*i)->setButtonState(mMouseRules.size() > 1, mMouseRules.size() < mMaxRules);
        }
        else
        {
            (*i)->setButtonState(true, mMouseRules.size() < mMaxRules);
        }
    }
}


bool MouseRuleConfig::startElement(const QString &/*namespaceURI*/, const QString &/*localName*/, const QString &qName, const QXmlAttributes &atts)
{
    if (!mIsInsideMouseRuleConfig && qName.toUpper().compare("MOUSERULECONFIG") == 0)
    {
        mIsInsideMouseRuleConfig = true;
        return true;
    }
    else if (mIsInsideMouseRuleConfig && !mIsInsideMouseRule && qName.toUpper().compare("MOUSERULE") == 0)
    {
        QPoint pos(0, 0);
        EPositionMode posMode(CurrentPosition);
        quint32 interval(50u);
        EIntervalMode intervalMode(MillisecondsInterval);
        quint32 action(1u);
        EActionMode actionMode(ButtonAction);
        for (int i = 0; i < atts.count(); ++i)
        {
            QString key = atts.qName(i).toUpper();
            QString val = atts.value(i).toUpper();
            if (key.compare("X") == 0)
            {
                pos.setX(val.toInt());
            }
            else if (key.compare("Y") == 0)
            {
                pos.setY(val.toInt());
            }
            else if (key.compare("POSMODE") == 0)
            {
                if (val.compare("ABS") == 0)
                {
                    posMode = AbsolutePosition;
                }
                else if (val.compare("REL") == 0)
                {
                    posMode = RelativePosition;
                }
            }
            else if (key.compare("INTERVAL") == 0)
            {
                interval = val.toUInt();
            }
            else if (key.compare("INTERVALMODE") == 0)
            {
                if (val.compare("S") == 0)
                {
                    intervalMode = SecondsInterval;
                }
                else if (val.compare("M") == 0)
                {
                    intervalMode = MinutesInterval;
                }
                else if (val.compare("H") == 0)
                {
                    intervalMode = HoursInterval;
                }
            }
            else if (key.compare("ACTION") == 0)
            {
                action = val.toUInt();
            }
            else if (key.compare("ACTIONMODE") == 0)
            {
                if (val.compare("KEY") == 0)
                {
                    actionMode = KeyAction;
                }
                else if (val.compare("NONE") == 0)
                {
                    actionMode = NoAction;
                }
            }
        }
        MouseRule *origRule = new MouseRule(0, pos, posMode, interval, intervalMode, action, actionMode);
        addRule(origRule);
        delete origRule;
        origRule = 0;
        mIsInsideMouseRule = true;
        return true;
    }
    return false;
}


bool MouseRuleConfig::endElement(const QString& /*namespaceURI*/, const QString& /*localName*/, const QString& qName)
{
    if (mIsInsideMouseRuleConfig && qName.toUpper().compare("MOUSERULECONFIG") == 0)
    {
        mIsInsideMouseRuleConfig = false;
        return true;
    }
    else if (mIsInsideMouseRuleConfig && mIsInsideMouseRule && qName.toUpper().compare("MOUSERULE") == 0)
    {
        mIsInsideMouseRule = false;
        return true;
    }
    return false;
}

