#ifndef EDITORTABWIDGET_H
#define EDITORTABWIDGET_H

#include <QStack>
#include <QMap>
#include <QTabWidget>

#include "editor.h"
#include "project/project.h"

class EditorTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    EditorTabWidget(Project *project);

    void addEditor(Editor *editor);
    Editor *currentEditor() const;
    Editor *editor(int i) const;

public slots:
    void addFileEditor(const QString &filePath);

    void closeEditor(int id=-1);
    void saveCurrentEditor();
    void saveAllEditor();

    void nextTab();
    void previousTab();

signals:
    void editorChange(Editor *editor);
    void currentEditorModified(Editor *editor);

protected slots:
    void updateTab();
    void activeTab(int id);

protected:
    bool eventFilter(QObject *o, QEvent *e);
    void registerAction();

    Project *_project;
    QStack<int> _activedTab;
    QMap<QString, Editor* > _mapPathEditor;
};

#endif // EDITORTABWIDGET_H