// _MyDocument_EOArchive_English.java
// Generated by EnterpriseObjects palette at 2006\u5e741\u670814\u65e5\u571f\u66dc\u65e5 9\u664236\u520619\u79d2Japan

import com.webobjects.eoapplication.*;
import com.webobjects.eocontrol.*;
import com.webobjects.eointerface.*;
import com.webobjects.eointerface.swing.*;
import com.webobjects.foundation.*;
import java.awt.*;
import javax.swing.*;
import javax.swing.border.*;
import javax.swing.table.*;
import javax.swing.text.*;

public class _MyDocument_EOArchive_English extends com.webobjects.eoapplication.EOArchive {
    IBHelpConnector _iBHelpConnector0, _iBHelpConnector1;
    com.webobjects.eointerface.swing.EOFrame _eoFrame0;
    com.webobjects.eointerface.swing.EOTextArea _nsTextView0, _nsTextView1;
    com.webobjects.eointerface.swing.EOTextField _nsTextField0, _nsTextField1;
    com.webobjects.eointerface.swing.EOView _nsBox0, _nsBox1;
    javax.swing.JButton _nsButton0, _nsButton1, _nsButton2;
    javax.swing.JCheckBox _nsButton3;
    javax.swing.JPanel _nsView0;

    public _MyDocument_EOArchive_English(Object owner, NSDisposableRegistry registry) {
        super(owner, registry);
    }

    protected void _construct() {
        Object owner = _owner();
        EOArchive._ObjectInstantiationDelegate delegate = (owner instanceof EOArchive._ObjectInstantiationDelegate) ? (EOArchive._ObjectInstantiationDelegate)owner : null;
        Object replacement;

        super._construct();

        _nsBox1 = (com.webobjects.eointerface.swing.EOView)_registered(new com.webobjects.eointerface.swing.EOView(), "NSView");
        _nsBox0 = (com.webobjects.eointerface.swing.EOView)_registered(new com.webobjects.eointerface.swing.EOView(), "NSBox1");
        _iBHelpConnector1 = (IBHelpConnector)_registered(new IBHelpConnector(), "");
        _nsButton3 = (javax.swing.JCheckBox)_registered(new javax.swing.JCheckBox("selected"), "NSButton3");
        _nsButton2 = (javax.swing.JButton)_registered(new javax.swing.JButton("Interrupt"), "NSButton1");
        _nsButton1 = (javax.swing.JButton)_registered(new javax.swing.JButton("Render Output Window"), "NSButton2");
        _iBHelpConnector0 = (IBHelpConnector)_registered(new IBHelpConnector(), "");
        _nsButton0 = (javax.swing.JButton)_registered(new javax.swing.JButton("Evaluate"), "NSButton");

        if ((delegate != null) && ((replacement = delegate.objectForOutletPath(this, "outputCounterField")) != null)) {
            _nsTextField1 = (replacement == EOArchive._ObjectInstantiationDelegate.NullObject) ? null : (com.webobjects.eointerface.swing.EOTextField)replacement;
            _replacedObjects.setObjectForKey(replacement, "_nsTextField1");
        } else {
            _nsTextField1 = (com.webobjects.eointerface.swing.EOTextField)_registered(new com.webobjects.eointerface.swing.EOTextField(), "NSTextField1111");
        }

        if ((delegate != null) && ((replacement = delegate.objectForOutletPath(this, "inputCounterField")) != null)) {
            _nsTextField0 = (replacement == EOArchive._ObjectInstantiationDelegate.NullObject) ? null : (com.webobjects.eointerface.swing.EOTextField)replacement;
            _replacedObjects.setObjectForKey(replacement, "_nsTextField0");
        } else {
            _nsTextField0 = (com.webobjects.eointerface.swing.EOTextField)_registered(new com.webobjects.eointerface.swing.EOTextField(), "NSTextField111");
        }

        if ((delegate != null) && ((replacement = delegate.objectForOutletPath(this, "textViewOut")) != null)) {
            _nsTextView1 = (replacement == EOArchive._ObjectInstantiationDelegate.NullObject) ? null : (com.webobjects.eointerface.swing.EOTextArea)replacement;
            _replacedObjects.setObjectForKey(replacement, "_nsTextView1");
        } else {
            _nsTextView1 = (com.webobjects.eointerface.swing.EOTextArea)_registered(new com.webobjects.eointerface.swing.EOTextArea(), "NSTextView");
        }

        if ((delegate != null) && ((replacement = delegate.objectForOutletPath(this, "textViewIn")) != null)) {
            _nsTextView0 = (replacement == EOArchive._ObjectInstantiationDelegate.NullObject) ? null : (com.webobjects.eointerface.swing.EOTextArea)replacement;
            _replacedObjects.setObjectForKey(replacement, "_nsTextView0");
        } else {
            _nsTextView0 = (com.webobjects.eointerface.swing.EOTextArea)_registered(new com.webobjects.eointerface.swing.EOTextArea(), "NSTextView");
        }

        if ((delegate != null) && ((replacement = delegate.objectForOutletPath(this, "window")) != null)) {
            _eoFrame0 = (replacement == EOArchive._ObjectInstantiationDelegate.NullObject) ? null : (com.webobjects.eointerface.swing.EOFrame)replacement;
            _replacedObjects.setObjectForKey(replacement, "_eoFrame0");
        } else {
            _eoFrame0 = (com.webobjects.eointerface.swing.EOFrame)_registered(new com.webobjects.eointerface.swing.EOFrame(), "Window");
        }

        _nsView0 = (JPanel)_eoFrame0.getContentPane();
    }

    protected void _awaken() {
        super._awaken();
        _nsButton3.addActionListener((com.webobjects.eointerface.swing.EOControlActionAdapter)_registered(new com.webobjects.eointerface.swing.EOControlActionAdapter(_owner(), "oxEvaluateSelected", _nsButton3), ""));
        _nsButton2.addActionListener((com.webobjects.eointerface.swing.EOControlActionAdapter)_registered(new com.webobjects.eointerface.swing.EOControlActionAdapter(_owner(), "oxInterrupt", _nsButton2), ""));
        _nsButton1.addActionListener((com.webobjects.eointerface.swing.EOControlActionAdapter)_registered(new com.webobjects.eointerface.swing.EOControlActionAdapter(_owner(), "oxRenderOutput", _nsButton1), ""));
        _nsButton0.addActionListener((com.webobjects.eointerface.swing.EOControlActionAdapter)_registered(new com.webobjects.eointerface.swing.EOControlActionAdapter(_owner(), "oxEvaluate", _nsButton0), ""));

        if (_replacedObjects.objectForKey("_eoFrame0") == null) {
            _connect(_eoFrame0, _owner(), "delegate");
        }

        if (_replacedObjects.objectForKey("_nsTextField1") == null) {
            _connect(_owner(), _nsTextField1, "outputCounterField");
        }

        if (_replacedObjects.objectForKey("_nsTextField0") == null) {
            _connect(_owner(), _nsTextField0, "inputCounterField");
        }

        if (_replacedObjects.objectForKey("_nsTextView1") == null) {
            _connect(_owner(), _nsTextView1, "textViewOut");
        }

        if (_replacedObjects.objectForKey("_eoFrame0") == null) {
            _connect(_owner(), _eoFrame0, "window");
        }

        if (_replacedObjects.objectForKey("_nsTextView0") == null) {
            _connect(_owner(), _nsTextView0, "textViewIn");
        }
    }

    protected void _init() {
        super._init();
        if (!(_nsBox0.getLayout() instanceof EOViewLayout)) { _nsBox0.setLayout(new EOViewLayout()); }
        _nsBox1.setSize(125, 1);
        _nsBox1.setLocation(2, 2);
        ((EOViewLayout)_nsBox0.getLayout()).setAutosizingMask(_nsBox1, EOViewLayout.MinYMargin);
        _nsBox0.add(_nsBox1);
        _nsBox0.setBorder(new com.webobjects.eointerface.swing._EODefaultBorder("", true, "Lucida Grande", 13, Font.PLAIN));
        _setFontForComponent(_nsButton3, "Lucida Grande", 13, Font.PLAIN);
        _setFontForComponent(_nsButton2, "Lucida Grande", 13, Font.PLAIN);
        _nsButton2.setMargin(new Insets(0, 2, 0, 2));
        _setFontForComponent(_nsButton1, "Lucida Grande", 13, Font.PLAIN);
        _nsButton1.setMargin(new Insets(0, 2, 0, 2));
        _setFontForComponent(_nsButton0, "Lucida Grande", 13, Font.PLAIN);
        _nsButton0.setMargin(new Insets(0, 2, 0, 2));

        if (_replacedObjects.objectForKey("_nsTextField1") == null) {
            _setFontForComponent(_nsTextField1, "Lucida Grande", 9, Font.PLAIN);
            _nsTextField1.setEditable(false);
            _nsTextField1.setOpaque(false);
            _nsTextField1.setText("output\n");
            _nsTextField1.setHorizontalAlignment(javax.swing.JTextField.LEFT);
            _nsTextField1.setSelectable(false);
            _nsTextField1.setEnabled(true);
            _nsTextField1.setBorder(null);
        }

        if (_replacedObjects.objectForKey("_nsTextField0") == null) {
            _setFontForComponent(_nsTextField0, "Lucida Grande", 9, Font.PLAIN);
            _nsTextField0.setEditable(false);
            _nsTextField0.setOpaque(false);
            _nsTextField0.setText("Input");
            _nsTextField0.setHorizontalAlignment(javax.swing.JTextField.LEFT);
            _nsTextField0.setSelectable(false);
            _nsTextField0.setEnabled(true);
            _nsTextField0.setBorder(null);
        }

        if (_replacedObjects.objectForKey("_nsTextView1") == null) {
            _nsTextView1.setEditable(true);
            _nsTextView1.setOpaque(true);
            _nsTextView1.setText("");
        }

        if (_replacedObjects.objectForKey("_nsTextView0") == null) {
            _nsTextView0.setEditable(true);
            _nsTextView0.setOpaque(true);
            _nsTextView0.setText("");
        }

        if (!(_nsView0.getLayout() instanceof EOViewLayout)) { _nsView0.setLayout(new EOViewLayout()); }
        _nsTextView0.setSize(467, 368);
        _nsTextView0.setLocation(37, 19);
        ((EOViewLayout)_nsView0.getLayout()).setAutosizingMask(_nsTextView0, EOViewLayout.WidthSizable | EOViewLayout.HeightSizable);
        _nsView0.add(_nsTextView0);
        _nsButton0.setSize(90, 26);
        _nsButton0.setLocation(4, -7);
        ((EOViewLayout)_nsView0.getLayout()).setAutosizingMask(_nsButton0, EOViewLayout.MaxXMargin | EOViewLayout.MaxYMargin);
        _nsView0.add(_nsButton0);
        _nsButton2.setSize(89, 26);
        _nsButton2.setLocation(419, -7);
        ((EOViewLayout)_nsView0.getLayout()).setAutosizingMask(_nsButton2, EOViewLayout.MinXMargin | EOViewLayout.MaxYMargin);
        _nsView0.add(_nsButton2);
        _nsTextView1.setSize(467, 160);
        _nsTextView1.setLocation(37, 395);
        ((EOViewLayout)_nsView0.getLayout()).setAutosizingMask(_nsTextView1, EOViewLayout.WidthSizable | EOViewLayout.MinYMargin);
        _nsView0.add(_nsTextView1);
        _nsBox0.setSize(531, 5);
        _nsBox0.setLocation(-7, 384);
        ((EOViewLayout)_nsView0.getLayout()).setAutosizingMask(_nsBox0, EOViewLayout.WidthSizable | EOViewLayout.MinYMargin);
        _nsView0.add(_nsBox0);
        _nsButton1.setSize(179, 26);
        _nsButton1.setLocation(305, 553);
        ((EOViewLayout)_nsView0.getLayout()).setAutosizingMask(_nsButton1, EOViewLayout.MinXMargin | EOViewLayout.MinYMargin);
        _nsView0.add(_nsButton1);
        _nsButton3.setSize(103, 31);
        _nsButton3.setLocation(96, -10);
        ((EOViewLayout)_nsView0.getLayout()).setAutosizingMask(_nsButton3, EOViewLayout.MaxXMargin | EOViewLayout.MaxYMargin);
        _nsView0.add(_nsButton3);
        _nsTextField0.setSize(43, 11);
        _nsTextField0.setLocation(-4, 27);
        ((EOViewLayout)_nsView0.getLayout()).setAutosizingMask(_nsTextField0, EOViewLayout.MaxXMargin | EOViewLayout.MaxYMargin);
        _nsView0.add(_nsTextField0);
        _nsTextField1.setSize(38, 12);
        _nsTextField1.setLocation(-5, 395);
        ((EOViewLayout)_nsView0.getLayout()).setAutosizingMask(_nsTextField1, EOViewLayout.MaxXMargin | EOViewLayout.MinYMargin);
        _nsView0.add(_nsTextField1);

        if (_replacedObjects.objectForKey("_eoFrame0") == null) {
            _nsView0.setSize(517, 589);
            _eoFrame0.setTitle("Window");
            _eoFrame0.setLocation(47, 123);
            _eoFrame0.setSize(517, 589);
        }
    }
}
