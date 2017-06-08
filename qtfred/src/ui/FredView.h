#pragma once

#include <QMainWindow>

#include <QtGui/QSurfaceFormat>

#include <memory>
#include <QtWidgets/QComboBox>
#include <mission/FredRenderer.h>

namespace fso {
namespace fred {

class Editor;
class RenderWindow;

namespace Ui {
class FredView;
}

class FredView : public QMainWindow
{
    Q_OBJECT

public:
    explicit FredView(QWidget *parent = 0);
    ~FredView();
    void setEditor(Editor* editor, FredRenderer* renderer);

	void loadMissionFile(const QString& pathName);

	RenderWindow* getRenderWindow();

public slots:
    void openLoadMissionDIalog();

	void newMission();

private slots:
    void on_actionShow_Background_triggered(bool checked);

    void on_actionShow_Horizon_triggered(bool checked);

    void on_actionShow_Grid_triggered(bool checked);

    void on_actionShow_Distances_triggered(bool checked);

    void on_actionShow_Coordinates_triggered(bool checked);

    void on_actionShow_Outlines_triggered(bool checked);

	void on_actionExit_triggered(bool);

	void on_mission_loaded(const std::string& filepath);

protected:
	void keyPressEvent(QKeyEvent* event) override;

	void keyReleaseEvent(QKeyEvent* event) override;

	bool eventFilter(QObject* watched, QEvent* event) override;

private:
	void addToRecentFiles(const QString& path);
	void updateRecentFileList();

	void recentFileOpened();

	std::unique_ptr<Ui::FredView> ui;

	std::unique_ptr<QComboBox> _shipClassBox;

    Editor* fred = nullptr;
	FredRenderer* _renderer = nullptr;
};


} // namespace fred
} // namespace fso
