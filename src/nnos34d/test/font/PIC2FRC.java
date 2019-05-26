package com.liu;  
  
import java.awt.Color;  
import java.awt.Font;  
import java.awt.event.ActionEvent;  
import java.awt.event.ActionListener;  
import java.awt.image.BufferedImage;  
import java.io.File;  
import java.io.IOException;  
  
import javax.imageio.ImageIO;  
import javax.swing.ComboBoxModel;  
import javax.swing.DefaultComboBoxModel;  
import javax.swing.ImageIcon;  
import javax.swing.JButton;  
import javax.swing.JColorChooser;  
import javax.swing.JComboBox;  
import javax.swing.JFileChooser;  
import javax.swing.JFrame;  
import javax.swing.JLabel;  
import javax.swing.JPanel;  
import javax.swing.JScrollPane;  
import javax.swing.JTextArea;  
import javax.swing.JTextField;  
  
public class Main extends JFrame {  
  
    /** 
     *  
     */  
    private static final long serialVersionUID = 2764274223326342315L;  
    private JTextArea showArea;  
    private JButton btnOk;  
  
    private JFileChooser jfc;  
    private JLabel jLabel3;  
    private JTextField point;  
    private JLabel jLabel2;  
    private JLabel jLabel1;  
    private JComboBox offsetbox;  
    private JScrollPane jScrollPane1;  
    private JButton btnColor;  
    private JPanel jpcolor;  
    private JButton choosefile;  
    private JTextField filepathArea;  
  
    private String YES = "*";  
  
    private String NO = " ";  
  
    private Integer[] cell = new Integer[] { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };  
  
    private JLabel jlImage = new JLabel();  
  
    private File image;  
  
    private Color color = new Color(0, 0, 0);  
  
    public Main() {  
        getContentPane().setLayout(null);  
        jfc = new JFileChooser("wenjian");  
  
        btnOk = new JButton();  
        getContentPane().add(btnOk);  
        btnOk.setText("ok");  
        btnOk.setBounds(869, 12, 59, 27);  
  
        jScrollPane1 = new JScrollPane();  
        getContentPane().add(jScrollPane1);  
        jScrollPane1.setBounds(458, 56, 470, 334);  
  
        showArea = new JTextArea();  
  
        jScrollPane1.setViewportView(showArea);  
        showArea.setBounds(458, 56, 470, 334);  
  
        filepathArea = new JTextField();  
        getContentPane().add(filepathArea);  
        filepathArea.setBounds(12, 12, 150, 27);  
  
        choosefile = new JButton();  
        getContentPane().add(choosefile);  
        choosefile.setText("\u56fe\u7247 ");  
        choosefile.setBounds(174, 12, 89, 27);  
  
        jpcolor = new JPanel();  
        getContentPane().add(jpcolor);  
        jpcolor.setBounds(294, 17, 25, 22);  
        jpcolor.setBackground(Color.BLACK);  
  
        btnColor = new JButton();  
        getContentPane().add(btnColor);  
        btnColor.setText("\u53d6\u8272");  
        btnColor.setBounds(331, 12, 82, 27);  
  
        ComboBoxModel offsetboxModel = new DefaultComboBoxModel(cell);  
        offsetbox = new JComboBox();  
        getContentPane().add(offsetbox);  
        offsetbox.setModel(offsetboxModel);  
        offsetbox.setBounds(470, 12, 62, 27);  
  
        jLabel1 = new JLabel();  
        getContentPane().add(jLabel1);  
        jLabel1.setText("px");  
        jLabel1.setBounds(537, 15, 18, 20);  
  
        jLabel2 = new JLabel();  
        getContentPane().add(jLabel2);  
        jLabel2.setText("\u504f\u79fb\u91cf");  
        jLabel2.setBounds(418, 15, 45, 20);  
  
        point = new JTextField();  
        getContentPane().add(point);  
        point.setBounds(675, 12, 45, 27);  
        point.setText(YES);  
  
        jLabel3 = new JLabel();  
        getContentPane().add(jLabel3);  
        jLabel3.setText("\u586b\u5145\u5b57\u7b26");  
        jLabel3.setBounds(592, 15, 80, 20);  
  
        btnColor.addActionListener(new ActionListener() {  
  
            @Override  
            public void actionPerformed(ActionEvent e) {  
                color = JColorChooser.showDialog(Main.this, "Color", color);  
                if (color == null) {  
                    color = Color.BLACK;  
                }  
                jpcolor.setBackground(color);  
                System.out.println(color.getRGB());  
            }  
        });  
  
        choosefile.addActionListener(new ActionListener() {  
            @Override  
            public void actionPerformed(ActionEvent e) {  
                getContentPane().remove(jlImage);  
                jfc.setDialogTitle("Open JPEG file");  
                jfc.showOpenDialog(jfc);  
                image = jfc.getSelectedFile();  
                if (image == null)  
                    return;  
                String path = image.getAbsolutePath();  
                filepathArea.setText(path);  
                jlImage = new JLabel(new ImageIcon(image.getAbsolutePath()));  
                getContentPane().add(jlImage);  
                jlImage.setBounds(0, 56, 470, 334);  
            }  
        });  
  
        btnOk.addActionListener(new ActionListener() {  
  
            @Override  
            public void actionPerformed(ActionEvent e) {  
                int offset = cell[offsetbox.getSelectedIndex()];  
                showArea.setText("");  
                String str = "";  
  
                BufferedImage bi = null;  
                try {  
                    bi = ImageIO.read(image);  
                } catch (IOException e1) {  
                    e1.printStackTrace();  
                }  
  
                for (int i = offset / 2; i < bi.getHeight(); i += offset) {  
                    str = "";  
                    for (int j = offset / 2; j < bi.getWidth(); j += offset) {  
                        int pixel = bi.getRGB(j, i);  
                        if (pixel > color.getRGB()) {  
                            str += NO;  
                        } else {  
                            str += YES;  
                        }  
                    }  
                    showArea.append(str + "\n");  
                    System.out.println(str);  
                }  
  
            }  
        });  
  
        this.setSize(969, 453);  
        this.setVisible(true);  
        this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);  
        this.setLocationRelativeTo(null);  
        this.setResizable(false);  
        showArea.setFont(new Font("黑体", Font.BOLD, 13));  
    }  
  
    public static void main(String[] args) {  
        new Main();  
    }  
  
}  