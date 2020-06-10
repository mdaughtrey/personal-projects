#include <buildlogger.h>

log4cpp::Category & buildLogger(std::string category)
{
  log4cpp::Appender * appender = new log4cpp::OstreamAppender(category, &std::cerr);
  log4cpp::Layout * layout = new log4cpp::BasicLayout();

  appender->setLayout(layout);

  log4cpp::Category & mainCategory = log4cpp::Category::getInstance(category);

  mainCategory.setAdditivity(false);
  mainCategory.setAppender(appender);
  mainCategory.setPriority(log4cpp::Priority::DEBUG);
  
  return log4cpp::Category::getInstance(category);
}

