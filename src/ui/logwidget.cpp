#include "logwidget.h"

#include "project/project.h"

#include <QDebug>
#include <QTextStream>

LogWidget::LogWidget(Project *project, QWidget *parent)
    : QTextBrowser(parent), _project(project)
{
    setTabChangesFocus(true);
    setReadOnly(true);
    setOpenLinks(false);
    _color = "30";

    _process = new QProcess(this);

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("TERM", "xterm");
    _process->setProcessEnvironment(env);

    connect(_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(readProcess()));
    connect(_process, SIGNAL(readyReadStandardOutput()), this, SLOT(readProcess()));
    connect(_process, SIGNAL(readyReadStandardError()), this, SLOT(readProcess()));
    _process->setWorkingDirectory(_project->rootPath());

    connect(this, SIGNAL(anchorClicked(QUrl)), this, SLOT(anchorClick(QUrl)));
}

LogWidget::~LogWidget()
{
    _process->terminate();
    delete _process;
}

void LogWidget::start(const QString &program, const QStringList &arguments)
{
    clear();
    document()->setDefaultStyleSheet("\
* {font-family: monospace;}\n\
.color30 { color: black; }\n\
.color31 { color: red; }\n\
.color32 { color: green; }\n\
.color33 { color: yellow; }\n\
.color34 { color: blue; }\n\
.color35 { color: magenta; }\n\
.color36 { color: cyan; }\n\
.color37 { color: white; }\n\
");
    _process->start(program, arguments);
}

void LogWidget::parseOutput(QByteArray data, bool error)
{
    QString html;
    QByteArray dataRead;
    QTextStream stream(&data);

    while (!stream.atEnd())
    {
        QString stringRead = stream.readLine();
        QString errorFormat = error ? " class=\"color31\"" : "";
        stringRead = "<br/><span" + errorFormat + ">" + stringRead.toHtmlEscaped() + "</span>";
        stringRead.replace(" ","&nbsp;");
        stringRead.replace(QRegExp("\\x001b\\[([0-9]+)m"),"</span><span class=\"color\\1\">");
        stringRead.replace(QRegExp("\\x001b\\(B\\x001b\\[m"),"</span><span>"); // reset color
        stringRead.replace("<span></span>","");

        stringRead.replace(QRegExp("([\\-\\._a-zA-Z/\\\\0-9]+\\.[a-zA-Z]+)(:[0-9]+)*(:[0-9]+)*"),"<a href=\"\\1\\2\\3\">\\1\\2\\3</a>");

        html.append(stringRead);
    }
    moveCursor(QTextCursor::End);
    insertHtml(html);
    moveCursor(QTextCursor::End);
}

void LogWidget::readProcess()
{
    parseOutput(_process->readAllStandardOutput(), false);
    parseOutput(_process->readAllStandardError(), true);
}

void LogWidget::anchorClick(const QUrl &link)
{
    emit openFileRequested(link.toString(QUrl::None));
}