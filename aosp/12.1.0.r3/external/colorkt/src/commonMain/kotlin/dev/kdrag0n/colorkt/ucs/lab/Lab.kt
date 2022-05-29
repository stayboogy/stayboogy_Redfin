package dev.kdrag0n.colorkt.ucs.lab

import dev.kdrag0n.colorkt.Color

/**
 * Common interface for color spaces that express color with the following 3 components:
 *   - L: perceived lightness
 *   - a: amount of green/red color
 *   - b: amount of blue/yellow color
 *
 * Implementations of this are usually uniform color spaces.
 *
 * It may be helpful to convert these colors to polar [dev.kdrag0n.colorkt.ucs.lch.Lch] representations
 * for easier manipulation.
 */
public interface Lab : Color {
    /**
     * Perceived lightness component.
     */
    public val L: Double

    /**
     * Green/red color component.
     */
    public val a: Double

    /**
     * Blue/yellow color component.
     */
    public val b: Double
}
