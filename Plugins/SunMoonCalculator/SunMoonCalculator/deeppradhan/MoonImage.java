package deeppradhan;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Image;
import java.awt.RenderingHints;
import java.awt.Toolkit;
import java.awt.geom.AffineTransform;

import javax.swing.JPanel;

/** Container to display a graphical representation of the Moon
 * @author Deep Pradhan, India, e-mail: wiz_kid_o3@yahoo.com
 * @version 26 May 2017*/
class MoonImage extends JPanel {

	/** Default constructor called when created*/
	MoonImage() {
		this.setPreferredSize(new Dimension(MOON_SIZE + MOON_PADDING * 2, MOON_SIZE + MOON_PADDING * 2));
	}

	/** Update MoonImage with a base moon image, shadow image and angle to rotate both by*/
	public void update(double phaseAngle, double angleMoon, double angleShadow) {
		this.phaseAngle = (int) Math.round(phaseAngle);
		this.angleMoon = (int) Math.round(angleMoon);
		this.angleShadow = (int) Math.round(angleShadow);
		repaint();
	}

	@Override
	public void paintComponent(Graphics graphic) {
		super.paintComponent(graphic);

		// RenderingHints to prefer quality over performance
	    RenderingHints rh = new RenderingHints(
	             RenderingHints.KEY_RENDERING,
	             RenderingHints.VALUE_RENDER_QUALITY);
	    

		Graphics2D graphics2d = (Graphics2D) graphic;
		graphics2d.addRenderingHints(rh);

		// Draw the Component black
		graphics2d.setColor(Color.BLACK);
		graphics2d.fillRect(0, 0, MOON_SIZE + MOON_PADDING * 2, MOON_SIZE + MOON_PADDING * 2);

		// Transform the base Moon image with rotation and draw
		AffineTransform at = new AffineTransform();
        at.translate(MOON_SIZE / 2 + MOON_PADDING, MOON_SIZE / 2 + MOON_PADDING);
        at.rotate(Math.toRadians(angleMoon));
        at.translate(-MOON_SIZE / 2, -MOON_SIZE / 2);
        graphics2d.drawImage(IMAGE_MOON, at, this);

		int phaseBy10 = Math.round(phaseAngle / 10);
		// Transform the shadow image and draw (unless Full Moon)
		if (phaseBy10 != 0) {
			at = new AffineTransform();
            at.translate(MOON_SIZE / 2 + MOON_PADDING, MOON_SIZE / 2 + MOON_PADDING);
            at.rotate(Math.toRadians(angleShadow));
            at.translate(-MOON_SIZE / 2, -MOON_SIZE / 2);
            graphics2d.drawImage(IMAGE_MASK[phaseBy10 - 1], at, this);
		}
	}

	/** Phase Angle which is related to the illumination*/
	private int phaseAngle = 0;

	/** Angle of rotation for Moon image*/
	private int angleMoon = 0;

	/** Angle of rotation for shadow image*/
	private int angleShadow = 0;

	private static final long serialVersionUID = 1L;

	/** Padding around the Moon image*/
	private final int MOON_PADDING = 10;

	/** Size (width/height) of Moon image*/
	private final int MOON_SIZE = 160;

	/** Image of Full Moon*/
	private final Image IMAGE_MOON = Toolkit.getDefaultToolkit().createImage(MoonImage.class.getResource("/resources/moon.png"));

	/** Images of shadow/masks*/
	private final Image IMAGE_MASK[] = {Toolkit.getDefaultToolkit().createImage(MoonImage.class.getResource("/resources/mask_010.png")),
			Toolkit.getDefaultToolkit().createImage(MoonImage.class.getResource("/resources/mask_020.png"))	,
			Toolkit.getDefaultToolkit().createImage(MoonImage.class.getResource("/resources/mask_030.png")),
			Toolkit.getDefaultToolkit().createImage(MoonImage.class.getResource("/resources/mask_040.png")),
			Toolkit.getDefaultToolkit().createImage(MoonImage.class.getResource("/resources/mask_050.png")),
			Toolkit.getDefaultToolkit().createImage(MoonImage.class.getResource("/resources/mask_060.png")),
			Toolkit.getDefaultToolkit().createImage(MoonImage.class.getResource("/resources/mask_070.png")),
			Toolkit.getDefaultToolkit().createImage(MoonImage.class.getResource("/resources/mask_080.png")),
			Toolkit.getDefaultToolkit().createImage(MoonImage.class.getResource("/resources/mask_090.png")),
			Toolkit.getDefaultToolkit().createImage(MoonImage.class.getResource("/resources/mask_100.png")),
			Toolkit.getDefaultToolkit().createImage(MoonImage.class.getResource("/resources/mask_110.png")),
			Toolkit.getDefaultToolkit().createImage(MoonImage.class.getResource("/resources/mask_120.png")),
			Toolkit.getDefaultToolkit().createImage(MoonImage.class.getResource("/resources/mask_130.png")),
			Toolkit.getDefaultToolkit().createImage(MoonImage.class.getResource("/resources/mask_140.png")),
			Toolkit.getDefaultToolkit().createImage(MoonImage.class.getResource("/resources/mask_150.png")),
			Toolkit.getDefaultToolkit().createImage(MoonImage.class.getResource("/resources/mask_160.png")),
			Toolkit.getDefaultToolkit().createImage(MoonImage.class.getResource("/resources/mask_170.png")),
			Toolkit.getDefaultToolkit().createImage(MoonImage.class.getResource("/resources/mask_180.png"))};
}