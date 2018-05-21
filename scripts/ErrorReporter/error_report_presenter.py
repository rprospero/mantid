from mantid.kernel import ErrorReporter, UsageService
from mantid.kernel import Logger
error_log = Logger("error")


class ErrorReporterPresenter():
    def __init__(self, view, exit_code):
        self._view = view
        self._exit_code = exit_code
        self._view.connect_signal(self.error_handler)
        self._view.show()

    def error_handler(self, continue_working, share, name, email):
        if share == 0:
            errorReporter = ErrorReporter(
                "mantidplot",UsageService.getUpTime(), UsageService.isEnabled(), self._exit_code,
                True, str(name), str(email))
            errorReporter.sendErrorReport()
        elif share == 1:
            errorReporter = ErrorReporter(
                "mantidplot",UsageService.getUpTime(), UsageService.isEnabled(), self._exit_code,
                False, str(name), str(email))
            errorReporter.sendErrorReport()
  
        if not continue_working:
            error_log.error("Terminated by user.")
            self._view.quit()
        else:
            error_log.error("Continue working.")
