import sys
import socket
from log import log
import json

# 类：connection
class connection:
    # 构造函数
    def __init__(self, remote_port, remote_ip, local_port, log_obj: log, local_ip=socket.gethostname()):
        #配置本地端口和ip，远程端口和ip
        self._socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self._remote_ip = remote_ip
        self._remote_port = remote_port
        self._local_ip = local_ip
        self._local_port = local_port
        self._log_obj = log_obj

    # 函数：client_initialize(self)
    # 作用：客户端初始化连接，包括绑定本地ip+端口，和与远程服务器ip+端口建立连接
    # 输入列表：[无]
    # 返回值：无
    def client_initialize(self):
        self._log_obj.print_log("尝试连接到服务器：" + self._remote_ip + ":" + str(self._remote_port))
        try:
            #将socket绑定到本地ip和端口上
            self._socket.bind((self._local_ip, self._local_port))
            #与远程ip和端口建立链接
            self._socket.connect((self._remote_ip, self._remote_port))
        except socket.error as msg:
            self._log_obj.print_log(msg)
            sys.exit(1)
        self._log_obj.print_log("成功与服务器建立连接")

    # 函数：encode_msg()
    # 作用：将python类型格式化为json字符串
    # 输入列表：[python_msg: 要被转化为json字符串的python类型]
    # 返回值：json字符串
    @staticmethod
    def encode_msg(self, python_type):
        json_data = json.dump(python_type)
        return json_data

    # 函数：decode_msg()
    # 作用：将json字符串还原为python类型
    # 输入列表：[json_msg: 要被转化为python类型的json字符串]
    # 返回值：python类型
    @staticmethod
    def decode_msg(self, json_msg):
        python_type = json.loads(json_msg)
        return python_type
