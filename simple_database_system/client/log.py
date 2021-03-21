import sys
import time

class log:
    def __init__(self, enable: bool):
        self._enabled = enable

    def print_log(self, log_info: str):
        if self._enabled:
            print(time.strftime("[%Y-%m-%d %H:%M:%S] Log Info: ", time.localtime()) + log_info)
