@file:Suppress("OPT_IN_USAGE")

import org.jetbrains.kotlin.gradle.idea.tcs.extras.isCommonizedKey
import org.jetbrains.kotlin.gradle.internal.platform.wasm.WasmPlatforms.wasmJs

plugins {
    alias(libs.plugins.kotlin.multiplatform)
}

java {
    toolchain {
        languageVersion = JavaLanguageVersion.of(17)
    }
}

kotlin {
    wasmJs {
        binaries.library()
    }
}