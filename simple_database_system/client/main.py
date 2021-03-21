import sys
import _thread
import connection
import log
from PyQt5.QtWidgets import QApplication, QWidget

if __name__ == '__main__':
    #创建日志记录对象
    log_obj = log.log(True)

    #初始化连接
    conn = connection.connection(remote_port=12345, remote_ip='127.0.0.1', local_port=12344, log_obj=log_obj)
    conn.client_initialize()

    """
        app = QApplication(sys.argv)
        w = QWidget()
        w.resize(250, 150)
        w.move(300, 300)
        w.setWindowTitle('Simple')
        w.show()
        sys.exit(app.exec_())
    """
