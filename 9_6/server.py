from flask import Flask, render_template, request
from flask_socketio import SocketIO, join_room, leave_room, emit, disconnect
import uuid

# 初始化Flask应用
app = Flask(__name__)
app.config['SECRET_KEY'] = 'secret!'  # 生产环境中应使用更安全的密钥
socketio = SocketIO(app, cors_allowed_origins="*")  # 允许跨域访问

# 存储在线用户信息: {username: sid}
online_users = {}
# 存储房间信息: {room_id: {name: room_name, members: [username1, username2]}}
rooms = {}

@app.route('/')
def index():
    """聊天页面"""
    return render_template('index.html')

@socketio.on('connect')
def handle_connect():
    """处理客户端连接"""
    print(f"客户端已连接: {request.sid}")
    emit('connection_response', {'message': '连接成功，请登录'})

@socketio.on('login')
def handle_login(data):
    """处理用户登录"""
    username = data.get('username')
    
    if not username:
        emit('login_response', {'success': False, 'message': '用户名不能为空'})
        return
        
    if username in online_users:
        emit('login_response', {'success': False, 'message': '用户名已被使用'})
        return
        
    # 记录用户信息
    online_users[username] = request.sid
    print(f"用户 {username} 已登录")
    
    # 通知登录成功
    emit('login_response', {
        'success': True, 
        'message': f'欢迎 {username} 加入聊天！',
        'username': username,
        'online_users': list(online_users.keys())
    })
    
    # 通知其他用户有新用户加入
    emit('user_joined', {
        'username': username,
        'online_users': list(online_users.keys())
    }, broadcast=True, include_self=False)

@socketio.on('send_message')
def handle_message(data):
    """处理发送消息"""
    username = data.get('username')
    message = data.get('message')
    room = data.get('room', 'general')  # 默认公共房间
    
    if not username or not message:
        return
        
    print(f"{username} 发送消息: {message}")
    
    # 广播消息
    if room == 'general':
        # 公共消息
        emit('new_message', {
            'username': username,
            'message': message,
            'timestamp': data.get('timestamp')
        }, broadcast=True)
    else:
        # 房间消息
        emit('new_message', {
            'username': username,
            'message': message,
            'room': room,
            'timestamp': data.get('timestamp')
        }, room=room)

@socketio.on('create_room')
def handle_create_room(data):
    """创建房间"""
    room_name = data.get('room_name')
    username = data.get('username')
    
    if not room_name:
        emit('room_response', {'success': False, 'message': '房间名不能为空'})
        return
        
    room_id = str(uuid.uuid4())[:8]  # 生成短房间ID
    rooms[room_id] = {
        'name': room_name,
        'members': [username]
    }
    
    # 加入房间
    join_room(room_id)
    
    emit('room_response', {
        'success': True,
        'message': f'房间 {room_name} 创建成功',
        'room_id': room_id,
        'room_name': room_name,
        'rooms': rooms
    })
    
    # 通知其他用户有新房间创建
    emit('room_created', {
        'room_id': room_id,
        'room_name': room_name,
        'creator': username
    }, broadcast=True, include_self=False)

@socketio.on('join_room')
def handle_join_room(data):
    """加入房间"""
    room_id = data.get('room_id')
    username = data.get('username')
    
    if room_id not in rooms:
        emit('room_response', {'success': False, 'message': '房间不存在'})
        return
        
    # 加入房间
    join_room(room_id)
    rooms[room_id]['members'].append(username)
    
    emit('room_response', {
        'success': True,
        'message': f'已加入房间 {rooms[room_id]["name"]}',
        'room_id': room_id,
        'room_name': rooms[room_id]['name'],
        'members': rooms[room_id]['members']
    })
    
    # 通知房间内其他成员
    emit('user_joined_room', {
        'username': username,
        'room_id': room_id,
        'members': rooms[room_id]['members']
    }, room=room_id, include_self=False)

@socketio.on('disconnect')
def handle_disconnect():
    """处理客户端断开连接"""
    # 找到断开连接的用户
    username = None
    for user, sid in online_users.items():
        if sid == request.sid:
            username = user
            break
            
    if username:
        # 从在线用户列表移除
        del online_users[username]
        print(f"用户 {username} 已断开连接")
        
        # 从所有房间移除
        for room_id in list(rooms.keys()):
            if username in rooms[room_id]['members']:
                rooms[room_id]['members'].remove(username)
                # 如果房间为空则删除
                if not rooms[room_id]['members']:
                    del rooms[room_id]
                else:
                    # 通知房间内其他成员
                    emit('user_left_room', {
                        'username': username,
                        'room_id': room_id,
                        'members': rooms[room_id]['members']
                    }, room=room_id)
        
        # 通知其他用户该用户已离开
        emit('user_left', {
            'username': username,
            'online_users': list(online_users.keys())
        }, broadcast=True, include_self=False)

if __name__ == '__main__':
    print("聊天服务器启动，访问 http://localhost:5000 进入聊天页面")
    socketio.run(app, debug=True, host='192.168.196.129', port=5000)
    
