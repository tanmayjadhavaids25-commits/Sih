package com.idr.demo.navigation

enum class NavigationMode(val label: String) {
    GNSS_INS("GNSS + INS"),
    AI_DEAD_RECKONING("DEAD RECKONING"),
    MAP_MATCHING("MAP MATCHING"),
    GNSS_RESTORING("GNSS RESTORING"),
    ERROR("ERROR")
}
