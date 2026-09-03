package com.idr.demo.sensors

class GnssManager {
    var available: Boolean = true
        private set

    fun setAvailable(available: Boolean) {
        this.available = available
    }
}
