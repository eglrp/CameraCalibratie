﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using cameracallibratie;
using Calibratie;
using WeifenLuo.WinFormsUI.Docking;

namespace CalibratieForms {
    public partial class CameraInfoWindow : DockContent {
        public PinholeCamera Camera {
            get { return _camera; }

            set {
                if (_camera != null) { _camera.PropertyChanged -= _camera_PropertyChanged; }
                _camera = value;
                if (_camera != null) { _camera.PropertyChanged += _camera_PropertyChanged; }
                updateForm(); 
            }
        }
        private PinholeCamera _camera;

        void _camera_PropertyChanged(object sender, PropertyChangedEventArgs e) {
            updateForm();
        }


        public static List<CameraInfoWindow> AllForms = new List<CameraInfoWindow>();
        public CameraInfoWindow() {
            InitializeComponent();
            AllForms.Add(this);
            this.Name += " " + AllForms.Count;
            this.Closed += (s, a) => { AllForms.Remove(this); };
        }

        public void updateForm() {
            if (Camera == null) {
                richTextBox1.Text = "no camera";
                return;
            }
            var cm = Camera.Intrinsics;
            richTextBox1.Text = String.Format("[{0:0.00} {1:0.00} {2:0.00}]\n" +
                                                "[{3:0.00} {4:0.00} {5:0.00}]\n" +
                                                "[{6:0.00} {7:0.00} {8:0.00}]"
                                                , cm.Mat[0, 0], cm.Mat[0, 1], cm.Mat[0, 2]
                                                , cm.Mat[1, 0], cm.Mat[1, 1], cm.Mat[1, 2]
                                                , cm.Mat[2, 0], cm.Mat[2, 1], cm.Mat[2, 2]);
            txt_fc1.Text = Camera.Intrinsics.fx.ToString();
            txt_fc2.Text = Camera.Intrinsics.fy.ToString();
            txt_cc1.Text = Camera.Intrinsics.cx.ToString();
            txt_cc2.Text = Camera.Intrinsics.cy.ToString();
            txt_ac.Text = Camera.Intrinsics.ac.ToString();

            txt_kc1.Text = Camera.DistortionR1.ToString();
            txt_kc2.Text = Camera.DistortionR2.ToString();
            txt_kc3.Text = Camera.DistortionT1.ToString();
            txt_kc4.Text = Camera.DistortionT2.ToString();
            txt_kc5.Text = Camera.DistortionR3.ToString();
        }
    }
}
