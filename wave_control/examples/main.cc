#include "Osc/sample/example_view.h"

#include "base/at_exit.h"
#include "base/command_line.h"
#include "base/process_util.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/base/ui_base_paths.h"
#include "views/focus/accelerator_handler.h"

int main(int argc, char** argv) {
#if defined(OS_WIN)
    OleInitialize(NULL);
#elif defined(OS_LINUX)
    // Initializes gtk stuff.
    g_thread_init(NULL);
    g_type_init();
    gtk_init(&argc, &argv);
#endif
    CommandLine::Init(argc, argv);

  base::EnableTerminationOnHeapCorruption();

  // The exit manager is in charge of calling the dtors of singleton objects.
  base::AtExitManager exit_manager;

  ui::RegisterPathProvider();
  ui::ResourceBundle::InitSharedInstance("en-US");

  MessageLoopForUI main_message_loop;

  // views::TestViewsDelegate delegate;

  // We do not use this header: chrome/common/chrome_switches.h
  // because that would create a bad dependency back on Chrome.
  views::Widget::SetPureViews(CommandLine::ForCurrentProcess()->
                              HasSwitch("use-pure-views"));

  scoped_ptr<ExampleView> view(ExampleView::CreateInstance());
  view->Init();

  views::AcceleratorHandler accelerator_handler;
  MessageLoopForUI::current()->Run(&accelerator_handler);

#if defined(OS_WIN)
    OleUninitialize();
#endif
    return 0;
}