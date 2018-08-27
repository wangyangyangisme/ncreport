#ifndef _NCICONFACTORYBASE_H_
#define _NCICONFACTORYBASE_H_


#include <QtCore/QObject>
#include <QtGui/QIcon>


class NCIconFactoryBase : public QObject
{
public:
	NCIconFactoryBase(QObject* parent = 0);

	virtual QIcon findIcon(QString iconName) const = 0;
};


#endif // !_NCICONFACTORYBASE_H_
