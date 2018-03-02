#ifndef MOUSERULECONFIG_H
#define MOUSERULECONFIG_H


#include <QObject>
#include <QXmlDefaultHandler>
#include "MouseRule.hpp"
class MouseRobot;
typedef QList<MouseRule*> MouseRules;


class MouseRuleObserver
{
public:
    virtual ~MouseRuleObserver() { }
    virtual void ruleAdded(MouseRule &rule) = 0;
    virtual void ruleRemoved(MouseRule &rule) = 0;
};


class MouseRuleConfig : public QObject, QXmlDefaultHandler
{
    Q_OBJECT

public:
    MouseRuleConfig(MouseRuleObserver *observer);
    const MouseRules &rules() const;
    void addRule(const MouseRule *origRule = 0);
    void removeRule(MouseRule *rule);

public slots:
    void invoke(MouseRobot &robot);
    void load(const QString &fileName);
    void save(const QString &fileName);

protected:
    void update();
    bool startElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &atts);
    bool endElement(const QString& namespaceURI, const QString& localName, const QString& qName);

private:
    MouseRuleObserver *mObserver;
    MouseRules mMouseRules;
    const qint32 mMaxRules;
    bool mIsInsideMouseRuleConfig;
    bool mIsInsideMouseRule;
};

#endif // MOUSERULECONFIG_H
