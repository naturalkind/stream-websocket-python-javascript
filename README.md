# stream-websocket-python-javascript
<h3><strong>Сервер</strong> - c++/python:</h3>
   <li>получаю нужное окно C++</li> 
   <li>данные окна отправляю в python</li>
   <li>отправляю клиенту</li>
<h3><strong>Клиент</strong> - javascript:</h3>
   <li>получаю даные</li>
# Запуск:

компиляция

```
g++ -c -fPIC lib.cpp -o lib -lX11 $(pkg-config opencv --cflags --libs) -I/usr/include/python2.7 -I/usr/lib/python2.7;
g++ -shared -Wl,-soname,lib.so -o lib.so lib -lopencv_core -lopencv_highgui -lopencv_imgproc;
```

старт

```
python svs.py
```
