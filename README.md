# Enhanced QCache

See [Qt official documentation](https://doc.qt.io/qt-5/qcache.html) for further details about QCache.
The original class has some significant disadvantages. The current one eliminates them. Features:
* No raw pointers
* You can use any type of smart pointers (shared-like presumably)
* You can specify custom object maker
