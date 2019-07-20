package deeppradhan;

import java.awt.EventQueue;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.FocusEvent;
import java.awt.event.FocusListener;
import java.text.DecimalFormat;
import java.text.DecimalFormatSymbols;
import java.text.NumberFormat;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.Locale;
import java.util.TimeZone;
import java.util.prefs.Preferences;

import javax.swing.DefaultComboBoxModel;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JFormattedTextField;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JSpinner;
import javax.swing.SpinnerDateModel;
import javax.swing.UIManager;
import javax.swing.border.TitledBorder;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import tomasalonso.SunMoonCalculator;

/** Basic application to show information of Sun, Moon including a graphic representation of the Moon
 * Utilizing Sun/Moon ephemerides (SunMoonCalculator) by T. Alonso Albi.
 * @author Deep Pradhan, India, e-mail: wiz_kid_o3@yahoo.com
 * @version 26 May 2017*/
public class App {

	/**  Launch the application */
	public static void main(String[] args) {
		try {
			UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
		} catch (Exception e) {
			e.printStackTrace();
		}
		EventQueue.invokeLater(new Runnable() {
			public void run() {
				try {
					App window = new App();
					window.frameSunMoonInfo.setVisible(true);
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		});
	}

	/** Create the application */
	public App() {

		// Number format for Lon/Lat - 123.123
		numberFormatFraction_3_4 = new DecimalFormat("###.####"); //NumberFormat.getNumberInstance();
		//numberFormatFraction_3_4.setGroupingUsed(false);
		//numberFormatFraction_3_4.setMaximumFractionDigits(4);
		//numberFormatFraction_3_4.setMaximumIntegerDigits(3);
		//numberFormatFraction_3_4.setMinimumFractionDigits(4);

		frameSunMoonInfo = new JFrame();
		frameSunMoonInfo.setResizable(false);
		frameSunMoonInfo.setTitle("Sun Moon Info");
		frameSunMoonInfo.setLocationByPlatform(true);
		frameSunMoonInfo.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

		// Get the current Calendar and TimeZone
		calendarSunMoon = Calendar.getInstance();
		timeZoneSunMoonTimes = TimeZone.getDefault();
		float timeZoneOffsetHours = (float) timeZoneSunMoonTimes.getOffset(calendarSunMoon.getTimeInMillis()) / (60 * 60 * 1000);
		timeZoneString = String.format("%s%02d:%02d", Math.signum(timeZoneOffsetHours) >= 0 ? "+" : "-",
				(int) Math.abs(Math.floor(timeZoneOffsetHours)), (int) ((timeZoneOffsetHours - Math.floor(timeZoneOffsetHours)) * 60));

		GridBagLayout gblFrameSunMoonInfo = new GridBagLayout();
		gblFrameSunMoonInfo.columnWidths = new int[]{0, 0, 0, 0};
		gblFrameSunMoonInfo.rowHeights = new int[]{0, 0};
		gblFrameSunMoonInfo.columnWeights = new double[]{0.0, 0.0, 1.0, Double.MIN_VALUE};
		gblFrameSunMoonInfo.rowWeights = new double[]{1.0, Double.MIN_VALUE};
		frameSunMoonInfo.getContentPane().setLayout(gblFrameSunMoonInfo);
		
		panelObserver = new JPanel();
		panelObserver.setBorder(new TitledBorder(null, "Observer", TitledBorder.LEADING, TitledBorder.TOP, null, null));
		GridBagConstraints gbcPanelObserver = new GridBagConstraints();
		gbcPanelObserver.fill = GridBagConstraints.BOTH;
		gbcPanelObserver.insets = new Insets(2, 2, 2, 0);
		gbcPanelObserver.gridx = 0;
		gbcPanelObserver.gridy = 0;
		frameSunMoonInfo.getContentPane().add(panelObserver, gbcPanelObserver);
		GridBagLayout gblDateTimeLocation = new GridBagLayout();
		gblDateTimeLocation.columnWidths = new int[]{0, 0};
		gblDateTimeLocation.rowHeights = new int[]{0, 0, 0, 0, 0, 0, 0};
		gblDateTimeLocation.columnWeights = new double[]{0.0, Double.MIN_VALUE};
		gblDateTimeLocation.rowWeights = new double[]{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Double.MIN_VALUE};
		panelObserver.setLayout(gblDateTimeLocation);
		labelDateTime = new JLabel("Date/Time (UTC):");
		GridBagConstraints gbcLabelDateTime = new GridBagConstraints();
		gbcLabelDateTime.anchor = GridBagConstraints.WEST;
		gbcLabelDateTime.insets = new Insets(2, 2, 2, 0);
		gbcLabelDateTime.gridx = 0;
		gbcLabelDateTime.gridy = 0;
		panelObserver.add(labelDateTime, gbcLabelDateTime);

		labelDateTime.setText(String.format("Date/Time (UTC%s):", timeZoneString));

		spinnerDateTime = new JSpinner();
		spinnerDateModel = new SpinnerDateModel();
		spinnerDateTime.setModel(spinnerDateModel);
		spinnerDateTime.setToolTipText("Date in DD MMM YYYY, Time in HH:MM:SS (24 hour)");

		// Set the spinner format
		SimpleDateFormat format = ((JSpinner.DateEditor) spinnerDateTime.getEditor()).getFormat();
		format.applyPattern("dd MMMM yyyy HH:mm:ss");
		spinnerDateModel.setCalendarField(Calendar.DAY_OF_YEAR);

		GridBagConstraints gbcSpinnerDateTime = new GridBagConstraints();
		gbcSpinnerDateTime.fill = GridBagConstraints.HORIZONTAL;
		gbcSpinnerDateTime.insets = new Insets(0, 2, 2, 0);
		gbcSpinnerDateTime.gridx = 0;
		gbcSpinnerDateTime.gridy = 1;

		panelObserver.add(spinnerDateTime, gbcSpinnerDateTime);
		
		buttonDateTimeNow = new JButton("Now");
		GridBagConstraints gbcButtonNow = new GridBagConstraints();
		gbcButtonNow.fill = GridBagConstraints.HORIZONTAL;
		gbcButtonNow.insets = new Insets(0, 2, 2, 0);
		gbcButtonNow.gridx = 0;
		gbcButtonNow.gridy = 2;
		panelObserver.add(buttonDateTimeNow, gbcButtonNow);
		
		JPanel panelLocation = new JPanel();
		panelLocation.setBorder(new TitledBorder(null, "Location:", TitledBorder.LEADING, TitledBorder.TOP, null, null));
		GridBagConstraints gbcPanelLocation = new GridBagConstraints();
		gbcPanelLocation.insets = new Insets(0, 0, 2, 0);
		gbcPanelLocation.fill = GridBagConstraints.BOTH;
		gbcPanelLocation.gridx = 0;
		gbcPanelLocation.gridy = 3;
		panelObserver.add(panelLocation, gbcPanelLocation);
		GridBagLayout gblPanelLocation = new GridBagLayout();
		gblPanelLocation.columnWidths = new int[]{0, 0, 0};
		gblPanelLocation.rowHeights = new int[]{0, 0, 0};
		gblPanelLocation.columnWeights = new double[]{1.0, 1.0, Double.MIN_VALUE};
		gblPanelLocation.rowWeights = new double[]{0.0, 0.0, Double.MIN_VALUE};
		panelLocation.setLayout(gblPanelLocation);

		GridBagConstraints gbcLabelLongitude = new GridBagConstraints();
		gbcLabelLongitude.anchor = GridBagConstraints.WEST;
		gbcLabelLongitude.insets = new Insets(0, 0, 2, 2);
		gbcLabelLongitude.gridx = 0;
		gbcLabelLongitude.gridy = 0;
		panelLocation.add(new JLabel("Longitude:"), gbcLabelLongitude);
		
		GridBagConstraints gbcLabelLatitude = new GridBagConstraints();
		gbcLabelLatitude.anchor = GridBagConstraints.WEST;
		gbcLabelLatitude.insets = new Insets(0, 0, 2, 0);
		gbcLabelLatitude.gridx = 1;
		gbcLabelLatitude.gridy = 0;
		panelLocation.add(new JLabel("Latitude:"), gbcLabelLatitude);
		
		textFieldLongitude = new JFormattedTextField(numberFormatFraction_3_4);
		GridBagConstraints gbcTextFieldLongitude = new GridBagConstraints();
		gbcTextFieldLongitude.fill = GridBagConstraints.HORIZONTAL;
		gbcTextFieldLongitude.insets = new Insets(0, 0, 0, 2);
		gbcTextFieldLongitude.gridx = 0;
		gbcTextFieldLongitude.gridy = 1;
		panelLocation.add(textFieldLongitude, gbcTextFieldLongitude);
		textFieldLongitude.setText("0.000");
		
		textFieldLatitude = new JFormattedTextField(numberFormatFraction_3_4);
		GridBagConstraints gbcTextFieldLatitude = new GridBagConstraints();
		gbcTextFieldLatitude.fill = GridBagConstraints.HORIZONTAL;
		gbcTextFieldLatitude.gridx = 1;
		gbcTextFieldLatitude.gridy = 1;
		panelLocation.add(textFieldLatitude, gbcTextFieldLatitude);
		textFieldLatitude.setText("0.000");
		
		panelSun = new JPanel();
		panelSun.setBorder(new TitledBorder(null, "Sun", TitledBorder.LEADING, TitledBorder.TOP, null, null));
		GridBagConstraints gbcPanelSun = new GridBagConstraints();
		gbcPanelSun.fill = GridBagConstraints.BOTH;
		gbcPanelSun.insets = new Insets(2, 0, 2, 0);
		gbcPanelSun.gridx = 1;
		gbcPanelSun.gridy = 0;
		frameSunMoonInfo.getContentPane().add(panelSun, gbcPanelSun);
		GridBagLayout gblPanelSun = new GridBagLayout();
		gblPanelSun.columnWidths = new int[]{0, 0, 0};
		gblPanelSun.rowHeights = new int[]{0, 0, 0, 0, 0, 0, 0};
		gblPanelSun.columnWeights = new double[]{0.0, 0.0, Double.MIN_VALUE};
		gblPanelSun.rowWeights = new double[]{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Double.MIN_VALUE};
		panelSun.setLayout(gblPanelSun);
		GridBagConstraints gbcLabelSun = new GridBagConstraints();
		gbcLabelSun.anchor = GridBagConstraints.WEST;
		gbcLabelSun.insets = new Insets(2, 2, 2, 2);
		gbcLabelSun.gridx = 0;
		gbcLabelSun.gridy = 0;
		panelSun.add(new JLabel("Rise:"), gbcLabelSun);
				
		labelSunRise = new JLabel(BLANK_TIME);
		GridBagConstraints gbcLabelSunRise = new GridBagConstraints();
		gbcLabelSunRise.insets = new Insets(2, 2, 2, 2);
		gbcLabelSunRise.gridx = 1;
		gbcLabelSunRise.gridy = 0;
		panelSun.add(labelSunRise, gbcLabelSunRise);
		GridBagConstraints gbcLabelNoon = new GridBagConstraints();
		gbcLabelNoon.anchor = GridBagConstraints.WEST;
		gbcLabelNoon.insets = new Insets(2, 2, 2, 2);
		gbcLabelNoon.gridx = 0;
		gbcLabelNoon.gridy = 1;
		panelSun.add(new JLabel("Noon:"), gbcLabelNoon);
		
		labelSunNoon = new JLabel(BLANK_TIME);
		GridBagConstraints gbcLabelSunNoon = new GridBagConstraints();
		gbcLabelSunNoon.insets = new Insets(2, 2, 2, 2);
		gbcLabelSunNoon.gridx = 1;
		gbcLabelSunNoon.gridy = 1;
		panelSun.add(labelSunNoon, gbcLabelSunNoon);
		GridBagConstraints gbcLabelSunSet = new GridBagConstraints();
		gbcLabelSunSet.anchor = GridBagConstraints.WEST;
		gbcLabelSunSet.insets = new Insets(2, 2, 2, 2);
		gbcLabelSunSet.gridx = 0;
		gbcLabelSunSet.gridy = 2;
		panelSun.add(new JLabel("Set:"), gbcLabelSunSet);
		
		labelSunSet = new JLabel(BLANK_TIME);
		GridBagConstraints gbcLabelSunSet2 = new GridBagConstraints();
		gbcLabelSunSet2.insets = new Insets(2, 2, 2, 2);
		gbcLabelSunSet2.gridx = 1;
		gbcLabelSunSet2.gridy = 2;
		panelSun.add(labelSunSet, gbcLabelSunSet2);
		GridBagConstraints gbcLabelAzimuth = new GridBagConstraints();
		gbcLabelAzimuth.anchor = GridBagConstraints.WEST;
		gbcLabelAzimuth.insets = new Insets(2, 2, 2, 2);
		gbcLabelAzimuth.gridx = 0;
		gbcLabelAzimuth.gridy = 3;
		panelSun.add(new JLabel("Azimuth:"), gbcLabelAzimuth);
		
		labelSunAzimuth = new JLabel(BLANK);
		GridBagConstraints gbcLabelSunAzimuth2 = new GridBagConstraints();
		gbcLabelSunAzimuth2.insets = new Insets(2, 2, 2, 2);
		gbcLabelSunAzimuth2.gridx = 1;
		gbcLabelSunAzimuth2.gridy = 3;
		panelSun.add(labelSunAzimuth, gbcLabelSunAzimuth2);
		GridBagConstraints gbcLabelSunElevation = new GridBagConstraints();
		gbcLabelSunElevation.anchor = GridBagConstraints.WEST;
		gbcLabelSunElevation.insets = new Insets(2, 2, 2, 2);
		gbcLabelSunElevation.gridx = 0;
		gbcLabelSunElevation.gridy = 4;
		panelSun.add(new JLabel("Elevation:"), gbcLabelSunElevation);
		
		labelSunElevation = new JLabel(BLANK);
		GridBagConstraints gbcLabelSunElevation2 = new GridBagConstraints();
		gbcLabelSunElevation2.insets = new Insets(2, 2, 2, 2);
		gbcLabelSunElevation2.gridx = 1;
		gbcLabelSunElevation2.gridy = 4;
		panelSun.add(labelSunElevation, gbcLabelSunElevation2);
		GridBagConstraints gbcLabelSunDistance = new GridBagConstraints();
		gbcLabelSunDistance.insets = new Insets(2, 2, 2, 2);
		gbcLabelSunDistance.gridx = 0;
		gbcLabelSunDistance.gridy = 5;
		panelSun.add(new JLabel("Distance:"), gbcLabelSunDistance);
		
		labelSunDistance = new JLabel(BLANK);
		GridBagConstraints gbcLabelSunDistance2 = new GridBagConstraints();
		gbcLabelSunDistance2.insets = new Insets(2, 2, 2, 2);
		gbcLabelSunDistance2.gridx = 1;
		gbcLabelSunDistance2.gridy = 5;
		panelSun.add(labelSunDistance, gbcLabelSunDistance2);
		
		panelMoon = new JPanel();
		panelMoon.setBorder(new TitledBorder(null, "Moon", TitledBorder.LEADING, TitledBorder.TOP, null, null));
		GridBagConstraints gbcPanelMoon = new GridBagConstraints();
		gbcPanelMoon.insets = new Insets(2, 0, 2, 2);
		gbcPanelMoon.fill = GridBagConstraints.BOTH;
		gbcPanelMoon.gridx = 2;
		gbcPanelMoon.gridy = 0;
		frameSunMoonInfo.getContentPane().add(panelMoon, gbcPanelMoon);
		GridBagLayout gblPanelMoon = new GridBagLayout();
		gblPanelMoon.columnWidths = new int[]{0, 0, 0};
		gblPanelMoon.rowHeights = new int[]{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
		gblPanelMoon.columnWeights = new double[]{1.0, 1.0, Double.MIN_VALUE};
		gblPanelMoon.rowWeights = new double[]{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, Double.MIN_VALUE};
		panelMoon.setLayout(gblPanelMoon);
		GridBagConstraints gbcLabelMoonRise = new GridBagConstraints();
		gbcLabelMoonRise.fill = GridBagConstraints.HORIZONTAL;
		gbcLabelMoonRise.insets = new Insets(2, 2, 2, 2);
		gbcLabelMoonRise.gridx = 0;
		gbcLabelMoonRise.gridy = 0;
		panelMoon.add(new JLabel("Rise:"), gbcLabelMoonRise);
		
		labelMoonRise = new JLabel(BLANK_TIME);
		GridBagConstraints gbcLabelMoonRise2 = new GridBagConstraints();
		gbcLabelMoonRise2.insets = new Insets(2, 2, 2, 0);
		gbcLabelMoonRise2.gridx = 1;
		gbcLabelMoonRise2.gridy = 0;
		panelMoon.add(labelMoonRise, gbcLabelMoonRise2);
		GridBagConstraints gbcLabelMoonTransit = new GridBagConstraints();
		gbcLabelMoonTransit.fill = GridBagConstraints.HORIZONTAL;
		gbcLabelMoonTransit.insets = new Insets(2, 2, 2, 2);
		gbcLabelMoonTransit.gridx = 0;
		gbcLabelMoonTransit.gridy = 1;
		panelMoon.add(new JLabel("Transit:"), gbcLabelMoonTransit);
		
		labelMoonTransit = new JLabel(BLANK_TIME);
		GridBagConstraints gbcLabelMoonTransit2 = new GridBagConstraints();
		gbcLabelMoonTransit2.insets = new Insets(2, 2, 2, 0);
		gbcLabelMoonTransit2.gridx = 1;
		gbcLabelMoonTransit2.gridy = 1;
		panelMoon.add(labelMoonTransit, gbcLabelMoonTransit2);
		GridBagConstraints gbcLabelMoonSet = new GridBagConstraints();
		gbcLabelMoonSet.fill = GridBagConstraints.HORIZONTAL;
		gbcLabelMoonSet.insets = new Insets(2, 2, 2, 2);
		gbcLabelMoonSet.gridx = 0;
		gbcLabelMoonSet.gridy = 2;
		panelMoon.add(new JLabel("Set:"), gbcLabelMoonSet);
		
		labelMoonSet = new JLabel(BLANK_TIME);
		GridBagConstraints gbcLabelMoonSet2 = new GridBagConstraints();
		gbcLabelMoonSet2.insets = new Insets(2, 2, 2, 0);
		gbcLabelMoonSet2.gridx = 1;
		gbcLabelMoonSet2.gridy = 2;
		panelMoon.add(labelMoonSet, gbcLabelMoonSet2);
		GridBagConstraints gbcLabelMoonAzimuth = new GridBagConstraints();
		gbcLabelMoonAzimuth.fill = GridBagConstraints.HORIZONTAL;
		gbcLabelMoonAzimuth.insets = new Insets(2, 2, 2, 2);
		gbcLabelMoonAzimuth.gridx = 0;
		gbcLabelMoonAzimuth.gridy = 3;
		panelMoon.add(new JLabel("Azimuth:"), gbcLabelMoonAzimuth);
		
		labelMoonAzimuth = new JLabel(BLANK);
		GridBagConstraints gbcLabelMoonAzimuth2 = new GridBagConstraints();
		gbcLabelMoonAzimuth2.insets = new Insets(2, 2, 2, 0);
		gbcLabelMoonAzimuth2.gridx = 1;
		gbcLabelMoonAzimuth2.gridy = 3;
		panelMoon.add(labelMoonAzimuth, gbcLabelMoonAzimuth2);
		GridBagConstraints gbcLabelMoonElevation = new GridBagConstraints();
		gbcLabelMoonElevation.fill = GridBagConstraints.HORIZONTAL;
		gbcLabelMoonElevation.insets = new Insets(2, 2, 2, 2);
		gbcLabelMoonElevation.gridx = 0;
		gbcLabelMoonElevation.gridy = 4;
		panelMoon.add(new JLabel("Elevation:"), gbcLabelMoonElevation);
		
		labelMoonElevation = new JLabel(BLANK);
		GridBagConstraints gbcLabelMoonElevation2 = new GridBagConstraints();
		gbcLabelMoonElevation2.insets = new Insets(2, 2, 2, 0);
		gbcLabelMoonElevation2.gridx = 1;
		gbcLabelMoonElevation2.gridy = 4;
		panelMoon.add(labelMoonElevation, gbcLabelMoonElevation2);
		GridBagConstraints gbcLabelMoonDistance = new GridBagConstraints();
		gbcLabelMoonDistance.fill = GridBagConstraints.HORIZONTAL;
		gbcLabelMoonDistance.insets = new Insets(2, 2, 2, 2);
		gbcLabelMoonDistance.gridx = 0;
		gbcLabelMoonDistance.gridy = 5;
		panelMoon.add(new JLabel("Distance:"), gbcLabelMoonDistance);
		
		labelMoonDistance = new JLabel(BLANK);
		GridBagConstraints gbcLabelMoonDistance2 = new GridBagConstraints();
		gbcLabelMoonDistance2.insets = new Insets(2, 2, 2, 0);
		gbcLabelMoonDistance2.gridx = 1;
		gbcLabelMoonDistance2.gridy = 5;
		panelMoon.add(labelMoonDistance, gbcLabelMoonDistance2);
		GridBagConstraints gbcLabelMoonAge = new GridBagConstraints();
		gbcLabelMoonAge.fill = GridBagConstraints.HORIZONTAL;
		gbcLabelMoonAge.insets = new Insets(2, 2, 2, 2);
		gbcLabelMoonAge.gridx = 0;
		gbcLabelMoonAge.gridy = 6;
		panelMoon.add(new JLabel("Age:"), gbcLabelMoonAge);
		
		labelMoonAge = new JLabel(BLANK);
		GridBagConstraints gbcLabelMoonAge2 = new GridBagConstraints();
		gbcLabelMoonAge2.insets = new Insets(2, 2, 2, 0);
		gbcLabelMoonAge2.gridx = 1;
		gbcLabelMoonAge2.gridy = 6;
		panelMoon.add(labelMoonAge, gbcLabelMoonAge2);
		GridBagConstraints gbcLabelMoonIllumination = new GridBagConstraints();
		gbcLabelMoonIllumination.fill = GridBagConstraints.HORIZONTAL;
		gbcLabelMoonIllumination.insets = new Insets(2, 2, 2, 2);
		gbcLabelMoonIllumination.gridx = 0;
		gbcLabelMoonIllumination.gridy = 7;
		panelMoon.add(new JLabel("Illumination:"), gbcLabelMoonIllumination);
		
		labelMoonIllumination = new JLabel(BLANK);
		GridBagConstraints gbcLabelMoonIllumination2 = new GridBagConstraints();
		gbcLabelMoonIllumination2.insets = new Insets(2, 2, 2, 0);
		gbcLabelMoonIllumination2.gridx = 1;
		gbcLabelMoonIllumination2.gridy = 7;
		panelMoon.add(labelMoonIllumination, gbcLabelMoonIllumination2);
		
		GridBagConstraints gbcLabelMoonPhase = new GridBagConstraints();
		gbcLabelMoonPhase.fill = GridBagConstraints.HORIZONTAL;
		gbcLabelMoonPhase.insets = new Insets(2, 2, 2, 2);
		gbcLabelMoonPhase.gridx = 0;
		gbcLabelMoonPhase.gridy = 8;
		panelMoon.add(new JLabel("Phase:"), gbcLabelMoonPhase);
		
		labelMoonPhase = new JLabel(BLANK);
		GridBagConstraints gbcLabelMoonPhase2 = new GridBagConstraints();
		gbcLabelMoonPhase2.insets = new Insets(2, 2, 2, 0);
		gbcLabelMoonPhase2.gridx = 1;
		gbcLabelMoonPhase2.gridy = 8;
		panelMoon.add(labelMoonPhase, gbcLabelMoonPhase2);
		
	
	

		
		
		GridBagConstraints gbclabelPhaseAngle = new GridBagConstraints();
		gbclabelPhaseAngle.fill = GridBagConstraints.HORIZONTAL;
		gbclabelPhaseAngle.insets = new Insets(2, 2, 2, 2);
		gbclabelPhaseAngle.gridx = 0;
		gbclabelPhaseAngle.gridy = 9;
		panelMoon.add(new JLabel("PhaseAngle:"), gbclabelPhaseAngle);
		labelPhaseAngle = new JLabel(BLANK);
		
		GridBagConstraints gbclabelPhaseAngle2 = new GridBagConstraints();
		gbclabelPhaseAngle2.insets = new Insets(2, 2, 2, 0);
		gbclabelPhaseAngle2.gridx = 1;
		gbclabelPhaseAngle2.gridy = 9;
		panelMoon.add(labelPhaseAngle, gbclabelPhaseAngle2);
		

		GridBagConstraints gbclabelAngleMoon = new GridBagConstraints();
		gbclabelAngleMoon.fill = GridBagConstraints.HORIZONTAL;
		gbclabelAngleMoon.insets = new Insets(2, 2, 2, 2);
		gbclabelAngleMoon.gridx = 0;
		gbclabelAngleMoon.gridy = 10;
		panelMoon.add(new JLabel("AngleMoon:"), gbclabelAngleMoon);
		labelAngleMoon = new JLabel(BLANK);
		
		GridBagConstraints gbclabelAngleMoon2 = new GridBagConstraints();
		gbclabelAngleMoon2.insets = new Insets(2, 2, 2, 0);
		gbclabelAngleMoon2.gridx = 1;
		gbclabelAngleMoon2.gridy = 10;
		panelMoon.add(labelAngleMoon, gbclabelAngleMoon2);
		
		
		GridBagConstraints gbclabelAngleShadow = new GridBagConstraints();
		gbclabelAngleShadow.fill = GridBagConstraints.HORIZONTAL;
		gbclabelAngleShadow.insets = new Insets(2, 2, 2, 2);
		gbclabelAngleShadow.gridx = 0;
		gbclabelAngleShadow.gridy = 11;
		panelMoon.add(new JLabel("AngleShadow:"), gbclabelAngleShadow);
		labelAngleShadow = new JLabel(BLANK);
		
		GridBagConstraints gbcLabelMooBL2 = new GridBagConstraints();
		gbcLabelMooBL2.insets = new Insets(2, 2, 2, 0);
		gbcLabelMooBL2.gridx = 1;
		gbcLabelMooBL2.gridy = 11;
		panelMoon.add(labelAngleShadow, gbcLabelMooBL2);
		
		
		moonImage = new MoonImage();
		GridBagConstraints gbcPanelMoonImage = new GridBagConstraints();
		gbcPanelMoonImage.fill = GridBagConstraints.BOTH;
		gbcPanelMoonImage.gridwidth = 2;
		gbcPanelMoonImage.insets = new Insets(0, 0, 0, 2);
		gbcPanelMoonImage.gridx = 0;
		gbcPanelMoonImage.gridy = 12;
		panelMoon.add(moonImage, gbcPanelMoonImage);
		
		

		spinnerDateTime.addChangeListener(new ChangeListener() {
			public void stateChanged(ChangeEvent e) {
				update();
			}
		});
		buttonDateTimeNow.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				spinnerDateTime.setValue(new Date());
				update();
			}
		});
		
		GridBagConstraints gbcLabelSunMoonTimeReference = new GridBagConstraints();
		gbcLabelSunMoonTimeReference.anchor = GridBagConstraints.WEST;
		gbcLabelSunMoonTimeReference.insets = new Insets(2, 2, 2, 0);
		gbcLabelSunMoonTimeReference.gridx = 0;
		gbcLabelSunMoonTimeReference.gridy = 4;
		panelObserver.add(new JLabel("Sun/Moon Times with respect to:"), gbcLabelSunMoonTimeReference);
		
		comboBoxTimeZone = new JComboBox<String>();
		GridBagConstraints gbcComboBoxTimeZone = new GridBagConstraints();
		gbcComboBoxTimeZone.fill = GridBagConstraints.HORIZONTAL;
		gbcComboBoxTimeZone.insets = new Insets(0, 2, 0, 0);
		gbcComboBoxTimeZone.gridx = 0;
		gbcComboBoxTimeZone.gridy = 5;
		panelObserver.add(comboBoxTimeZone, gbcComboBoxTimeZone);
		comboBoxTimeZone.setModel(new DefaultComboBoxModel<String>(new String[] {
				"Device time (UTC" + timeZoneString + "Hours)", "Coordinated Universal Time (UTC)"}));
		comboBoxTimeZone.setSelectedIndex(0);

		textFieldLongitude.setText(Preferences.userNodeForPackage(App.class).get(KEY_OBSERVER_LONGITUDE, "0.000000"));
		textFieldLatitude.setText(Preferences.userNodeForPackage(App.class).get(KEY_OBSERVER_LATITUDE, "0.000000"));
		textFieldLongitude.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				textFieldLatitude.requestFocus();
			}
		});
		textFieldLongitude.addFocusListener(new FocusListener() {
			/** Value before change*/
			String value;

			@Override
			public void focusLost(FocusEvent e) {
				try {
					// Save if valid
					if (Math.abs(Double.parseDouble(textFieldLongitude.getText())) <= 180) {
						Preferences.userNodeForPackage(App.class).put(KEY_OBSERVER_LONGITUDE, textFieldLongitude.getText());
						update();
					} else
						messageDialog("Invalid Longitude", "Please specify a valid Longitude (-180\u00b0 to +180\u00b0)");
				} catch (Exception ex) {
					// Revert to saved value in case of a non numeric value
					textFieldLongitude.setText(value);
				}
			}

			@Override
			public void focusGained(FocusEvent e) {
				// Save value before change
				value = textFieldLongitude.getText();
			}
		});
		textFieldLatitude.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				comboBoxTimeZone.requestFocus();
			}
		});
		textFieldLatitude.addFocusListener(new FocusListener() {
			/** Value before change*/
			String value;

			@Override
			public void focusLost(FocusEvent e) {
				try {
					// Save if valid
					if (Math.abs(Double.parseDouble(textFieldLatitude.getText())) <= 90) {
						Preferences.userNodeForPackage(App.class).put(KEY_OBSERVER_LATITUDE, textFieldLatitude.getText());
						update();
					} else
						messageDialog("Invalid Latitude", "Please specify a valid Latitude (-90\u00b0 to +90\u00b0)");
				} catch (Exception ex) {
					// Revert to saved value in case of a non numeric value
					textFieldLatitude.setText(value);
				}
			}

			@Override
			public void focusGained(FocusEvent e) {
				// Save value before change
				value = textFieldLatitude.getText();
			}
		});

		comboBoxTimeZone.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				// Update the TimeZone value as per the selected zone
				timeZoneSunMoonTimes = TimeZone.getTimeZone(comboBoxTimeZone.getSelectedIndex() == 0 ?
						"GMT" + timeZoneString : "UTC");
				update();
			}
		});
		
		// Update the Time Zone ComboBox
		comboBoxTimeZone.setSelectedItem(timeZoneString);
		update();
	}

	/** Method to update the Sun/Moon Information*/
	private void update() {
			try {
				calendarSunMoon = Calendar.getInstance();
				calendarSunMoon.setTime((Date) spinnerDateTime.getValue());

				longitude = Double.parseDouble(textFieldLongitude.getText());
				latitude = Double.parseDouble(textFieldLatitude.getText());

				// Check if location is valid
				if (Math.abs(longitude) > 180 || Math.abs(latitude) > 90) {
					clearValues();
					return;
				}

				// Get Sun Moon Calculator object
				// Set Calendar to UTC
				calendarSunMoon.setTimeZone(TimeZone.getTimeZone("UTC"));
				sunMoonCalculator = new SunMoonCalculator(calendarSunMoon,
						Math.toRadians(longitude), Math.toRadians(latitude));
				sunMoonCalculator.calcSunAndMoon();

				// Sun details
				labelSunRise.setText(App.getTimeAsString(sunMoonCalculator.sunRise, timeZoneSunMoonTimes));
				labelSunNoon.setText(App.getTimeAsString(sunMoonCalculator.sunTransit, timeZoneSunMoonTimes));
				labelSunNoon.setToolTipText(String.format("Elevation: " + FORMAT_ANGLES, Math.toDegrees(sunMoonCalculator.sunTransitElev)));
				labelSunSet.setText(App.getTimeAsString(sunMoonCalculator.sunSet, timeZoneSunMoonTimes));
				labelSunAzimuth.setText(String.format(FORMAT_ANGLES, Math.toDegrees(sunMoonCalculator.sunAz)));
				labelSunElevation.setText(String.format(FORMAT_ANGLES, Math.toDegrees(sunMoonCalculator.sunEl)));
				labelSunDistance.setText(String.format("%05.4f AU", sunMoonCalculator.sunDist));

				// Moon details
				labelMoonAzimuth.setText(String.format(FORMAT_ANGLES, Math.toDegrees(sunMoonCalculator.moonAz)));
				labelMoonElevation.setText(String.format(FORMAT_ANGLES, Math.toDegrees(sunMoonCalculator.moonEl)));
				labelMoonDistance.setText(String.format("%,.0f Km", sunMoonCalculator.moonDist * SunMoonCalculator.AU));
				labelMoonAge.setText(String.format("%03.1f days", sunMoonCalculator.moonAge));
				labelMoonIllumination.setText(String.format("%03.1f%%", sunMoonCalculator.moonIll * 100));
				labelMoonPhase.setText(SunMoonCalculator.getMoonPhaseName(sunMoonCalculator.moonAge));
				
				labelPhaseAngle.setText(String.format(FORMAT_ANGLES, Math.toDegrees(Math.acos(-Math.cos(sunMoonCalculator.moonAge
						/ SunMoonCalculator.LUNAR_CYCLE_DAYS * 2 * Math.PI)))));
				labelAngleMoon.setText(String.format(FORMAT_ANGLES, Math.toDegrees(-(sunMoonCalculator.moonP - sunMoonCalculator.moonPar))));
				labelAngleShadow.setText(String.format(FORMAT_ANGLES, Math.toDegrees(-(sunMoonCalculator.moonBL - sunMoonCalculator.moonPar))));

				// Update Moon Image panel
				moonImage.update(Math.toDegrees(Math.acos(-Math.cos(sunMoonCalculator.moonAge
						/ SunMoonCalculator.LUNAR_CYCLE_DAYS * 2 * Math.PI))),
						Math.toDegrees(-(sunMoonCalculator.moonP - sunMoonCalculator.moonPar)),
						Math.toDegrees(-(sunMoonCalculator.moonBL - sunMoonCalculator.moonPar)));

				// Additional details on the image Tool Tip
				moonImage.setToolTipText(String.format("Position angle of axis: "
						+ FORMAT_ANGLES + ", Bright Limb angle: "
						+ FORMAT_ANGLES + ", Paralactic angle: "
						+ FORMAT_ANGLES,
						Math.toDegrees(sunMoonCalculator.moonP),
						Math.toDegrees(sunMoonCalculator.moonBL),
						Math.toDegrees(sunMoonCalculator.moonPar)));

//				// Get values for current day
//				double moonRise = sunMoonCalculator.moonRise,
//					moonTransit = sunMoonCalculator.moonTransit,
//					moonSet = sunMoonCalculator.moonSet,
//					moonTransitElevation = sunMoonCalculator.moonTransitElev;
//
//				// Get values for prev day
//				calendarSunMoon.add(Calendar.DATE, -1);
//				sunMoonCalculator = new SunMoonCalculator(calendarSunMoon,
//						Math.toRadians(longitude), Math.toRadians(latitude));
//				sunMoonCalculator.calcSunAndMoon();
//				double moonRisePrev = sunMoonCalculator.moonRise,
//						moonTransitPrev = sunMoonCalculator.moonTransit,
//						moonSetPrev = sunMoonCalculator.moonSet,
//						moonTransitElevationPrev = sunMoonCalculator.moonTransitElev;
//
//				// Get values for next day
//				calendarSunMoon.add(Calendar.DATE, 2);
//				sunMoonCalculator = new SunMoonCalculator(calendarSunMoon,
//						Math.toRadians(longitude), Math.toRadians(latitude));
//				sunMoonCalculator.calcSunAndMoon();
//				double moonRiseNext = sunMoonCalculator.moonRise,
//						moonTransitNext = sunMoonCalculator.moonTransit,
//						moonSetNext = sunMoonCalculator.moonSet,
//						moonTransitElevationNext = sunMoonCalculator.moonTransitElev;
//
//				// Reset to current day
//				calendarSunMoon.add(Calendar.DATE, -1);
//
//				// Set relevant Moon rise time
//				if (App.getCalendar(moonRiseNext, timeZoneSunMoonTimes).get(Calendar.DATE) == calendarSunMoon.get(Calendar.DATE))
//					moonRise = moonRiseNext;
//				if (App.getCalendar(moonRisePrev, timeZoneSunMoonTimes).get(Calendar.DATE) == calendarSunMoon.get(Calendar.DATE))
//					moonRise = moonRisePrev;
//				if (App.getCalendar(moonRise, timeZoneSunMoonTimes).get(Calendar.DATE) != calendarSunMoon.get(Calendar.DATE))
//					moonRise = Double.NaN;
//
//				// Set relevant Moon transit time & elevation
//				if (App.getCalendar(moonTransitNext, timeZoneSunMoonTimes).get(Calendar.DATE) == calendarSunMoon.get(Calendar.DATE)) {
//					moonTransit = moonTransitNext;
//					moonTransitElevation = moonTransitElevationNext;
//				}
//				if (App.getCalendar(moonTransitPrev, timeZoneSunMoonTimes).get(Calendar.DATE) == calendarSunMoon.get(Calendar.DATE)) {
//					moonTransit = moonTransitPrev;
//					moonTransitElevation = moonTransitElevationPrev;
//				}
//				if (App.getCalendar(moonTransit, timeZoneSunMoonTimes).get(Calendar.DATE) != calendarSunMoon.get(Calendar.DATE))
//					moonTransit = moonTransitElevation = Double.NaN;
//
//				// Set relevant Moon rise time
//				if (App.getCalendar(moonSetNext, timeZoneSunMoonTimes).get(Calendar.DATE) == calendarSunMoon.get(Calendar.DATE))
//					moonSet = moonSetNext;
//				if (App.getCalendar(moonSetPrev, timeZoneSunMoonTimes).get(Calendar.DATE) == calendarSunMoon.get(Calendar.DATE))
//					moonSet = moonSetPrev;
//				if (App.getCalendar(moonSet, timeZoneSunMoonTimes).get(Calendar.DATE) != calendarSunMoon.get(Calendar.DATE))
//					moonSet = Double.NaN;
//
//				labelMoonRise.setText(App.getTimeAsString(moonRise, timeZoneSunMoonTimes));
//				labelMoonTransit.setText(App.getTimeAsString(moonTransit, timeZoneSunMoonTimes));
//				labelMoonTransit.setToolTipText(Double.isNaN(moonTransitElevation) ?
//						null : String.format("Elevation: " + FORMAT_ANGLES, Math.toDegrees(moonTransitElevation)));
//				labelMoonSet.setText(App.getTimeAsString(moonSet, timeZoneSunMoonTimes));
				
				frameSunMoonInfo.pack();
			} catch (Exception e) {
				clearValues();
				e.printStackTrace();
				messageDialog("Exception", e.toString());
			}
	}

	/** Method to clear values in case of invalid input/results*/
	private void clearValues() {
		// Sun details
		labelSunRise.setText(BLANK_TIME);
		labelSunNoon.setText(BLANK_TIME);
		labelSunNoon.setToolTipText(null);
		labelSunSet.setText(BLANK_TIME);
		labelSunAzimuth.setText(BLANK);
		labelSunElevation.setText(BLANK);
		labelSunDistance.setText(BLANK);

		// Moon details
		labelMoonRise.setText(BLANK_TIME);
		labelMoonTransit.setText(BLANK_TIME);
		labelMoonTransit.setToolTipText(null);
		labelMoonSet.setText(BLANK_TIME);
		labelMoonAzimuth.setText(BLANK);
		labelMoonElevation.setText(BLANK);
		labelMoonDistance.setText(BLANK);
		labelMoonAge.setText(BLANK);
		labelMoonIllumination.setText(BLANK);
		labelMoonPhase.setText(BLANK);
		labelPhaseAngle.setText(BLANK);
		labelAngleMoon.setText(BLANK);
		labelAngleShadow.setText(BLANK);

		moonImage.update(0, 0, 0);
		frameSunMoonInfo.pack();
	}

	/** Method to return a Calendar for a specific Juliand date and TimeZone*/
	private static Calendar getCalendar(double jd, TimeZone timeZone) throws Exception {
		int[] date = SunMoonCalculator.getDate(jd);
	
		// Get a Calendar with UTC TimeZone
		Calendar calendar = Calendar.getInstance(TimeZone.getTimeZone("UTC"));
		calendar.set(date[0], date[1] - 1, date[2], date[3], date[4], date[5]);
		if (timeZone != null)
			calendar.add(Calendar.MILLISECOND, timeZone.getOffset(calendar.getTime().getTime()));
	
		return calendar;
	}

	/**
	 * Returns a date as a string.
	 * @param jd The Juliand day.
	 * @param timeZone The TimeZone to display time in
	 * @return The String.
	 * @throws Exception If the date does not exists.
	 */
	private static String getTimeAsString(double jd, TimeZone timeZone) throws Exception {
		if (jd == -1 || Double.isNaN(jd))
			return BLANK;
	
		Calendar calendar = App.getCalendar(jd, timeZone);
		return String.format(Locale.ENGLISH, "%tT", calendar);
	}

	/** Method to display a general purpose message dialog*/
	private void messageDialog(String title, String message) {
		JOptionPane.showMessageDialog(null, message, title, JOptionPane.DEFAULT_OPTION);
	}

	/** Observer Location (Longitude, Latitude; in degrees)*/
	private double longitude, latitude;

	/** Time Zone string representation (\u00b0HH:MM)*/
	private String timeZoneString;

	/** The Date/Time of Sun/Moon Info*/
	private Calendar calendarSunMoon;

	/** Number format for Lon/Lat*/
	private NumberFormat numberFormatFraction_3_4;

	/** The TimeZone to display time in*/
	private TimeZone timeZoneSunMoonTimes;

	private JButton buttonDateTimeNow;

	private JComboBox<String> comboBoxTimeZone;

	private JFrame frameSunMoonInfo;

	private JFormattedTextField textFieldLongitude, textFieldLatitude;

	private JLabel labelDateTime,
			labelSunRise,
			labelSunNoon,
			labelSunSet,
			labelSunAzimuth,
			labelSunElevation,
			labelSunDistance,
			labelMoonRise,
			labelMoonTransit,
			labelMoonSet,
			labelMoonAzimuth,
			labelMoonElevation,
			labelMoonDistance,
			labelMoonAge,
			labelMoonIllumination,
			labelMoonPhase,
			labelPhaseAngle,
			labelAngleMoon,
			labelAngleShadow;

	private JPanel panelObserver, panelSun, panelMoon;

	private SpinnerDateModel spinnerDateModel;
	private JSpinner spinnerDateTime;

	private SunMoonCalculator sunMoonCalculator;

	private MoonImage moonImage;

	/** Default blank texts*/
	private final static String BLANK = "-";

	private final String BLANK_TIME = "--:--:--";

	/** Format of angles*/
	private final String FORMAT_ANGLES = "%03.1f\u00b0";

	/** Keys of stored values*/
	private final String KEY_OBSERVER_LONGITUDE = "observerLongitude",
			KEY_OBSERVER_LATITUDE = "observerLatitude";
}
