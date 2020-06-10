#ifndef INCLUDED_BUILDLOGGER_H
#define INCLUDED_BUILDLOGGER_H

#include <log4cpp/Category.hh>
#include <log4cpp/CategoryStream.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/BasicLayout.hh>
#include <string>

#define LOG4CPP_DEBUG log.debugStream()
#define LOG4CPP_INFO log.infoStream()
#define LOG4CPP_NOTICE log.noticeStream()
#define LOG4CPP_WARN log.warnStream()
#define LOG4CPP_ERROR log.errorStream()
#define LOG4CPP_CRITICAL log.critStream()
#define LOG4CPP_ALERT log.alertStream()
#define LOG4CPP_EMERG log.emergStream()
#define LOG4CPP_FATAL log.fatalStream()
#define LOG4CPP_END log4cpp::CategoryStream::ENDLINE

log4cpp::Category & buildLogger(std::string category);

#endif // INCLUDED_BUILDLOGGER_H 
