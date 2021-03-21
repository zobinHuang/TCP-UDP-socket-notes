import sys

class login:
    #用户输入的用户名
    _username = ''

    #用户输入的密码
    _password = ''

    #登录状态机(登出：LOGOUT, 登录中：LOGGING, 已登录：LOGIN)
    _state_machine = 'LOGOUT'

    #用户权限等级(regular：普通用户，Manager：管理员)
    _user_authority = 'regular'

    def __init__(self, username, password):
        _username = username
        _password = password

    def get_authority(self):
        return self._user_authority
