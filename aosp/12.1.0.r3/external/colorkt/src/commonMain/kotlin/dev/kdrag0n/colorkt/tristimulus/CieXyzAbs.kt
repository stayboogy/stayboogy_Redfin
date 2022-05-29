package dev.kdrag0n.colorkt.tristimulus

import dev.kdrag0n.colorkt.Color
import dev.kdrag0n.colorkt.conversion.ConversionGraph
import kotlin.jvm.JvmName
import kotlin.jvm.JvmOverloads
import kotlin.jvm.JvmStatic
import kotlin.jvm.JvmSynthetic

/**
 * A color in the CIE XYZ tristimulus color space, with absolute luminance.
 * This is often used as an intermediate color space for uniform color spaces and color appearance models.
 *
 * @see dev.kdrag0n.colorkt.tristimulus.CieXyz
 */
public data class CieXyzAbs(
    /**
     * X component: mix of the non-negative CIE RGB curves.
     */
    val x: Double,

    /**
     * Y component: absolute luminance.
     */
    val y: Double,

    /**
     * Z component: approximately equal to blue from CIE RGB.
     */
    val z: Double,
) : Color {
    /**
     * Convert an absolute XYZ color to relative XYZ, using the specified reference white luminance.
     *
     * @return Color in relative XYZ
     */
    @JvmOverloads
    public fun toRel(luminance: Double = DEFAULT_SDR_WHITE_LUMINANCE): CieXyz = CieXyz(
        x = x / luminance,
        y = y / luminance,
        z = z / luminance,
    )

    public companion object {
        /**
         * Default absolute luminance used to convert SDR colors to absolute XYZ.
         * This effectively models the color being displayed on a display with a brightness of 200 nits (cd/m^2).
         */
        public const val DEFAULT_SDR_WHITE_LUMINANCE: Double = 200.0 // cd/m^2

        @JvmSynthetic
        internal fun register() {
            ConversionGraph.add<CieXyz, CieXyzAbs> { it.toAbs() }
            ConversionGraph.add<CieXyzAbs, CieXyz> { it.toRel() }
        }

        /**
         * Convert a relative XYZ color to absolute XYZ, using the specified reference white luminance.
         *
         * @return Color in absolute XYZ
         */
        @JvmStatic
        @JvmOverloads
        @JvmName("fromRel")
        public fun CieXyz.toAbs(luminance: Double = DEFAULT_SDR_WHITE_LUMINANCE): CieXyzAbs = CieXyzAbs(
            x = x * luminance,
            y = y * luminance,
            z = z * luminance,
        )
    }
}
