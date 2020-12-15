#!/usr/bin/env python
# coding: utf-8

# In[60]:


import sys, array
import ROOT

sys.path.append('notebooks/')
import MyStyle
MyStyle.set_my_style()


# In[249]:

ROOT.gROOT.ForceStyle(1)
style = ROOT.gStyle
style.SetPadLeftMargin(0.15)
style.SetPadRightMargin(0.15)
style.SetPadTopMargin(0.15)
style.SetPadBottomMargin(0.15)

style.SetTitleOffset(1.2,"xy")


# In[61]:


canvases = {}


# In[275]:


pref = 'plots/full_geom_v1/gen_muons/3002-100m/'
fname = pref+'hists.root'



histnames = [
    	'Energy',
	'Zenith',
	'Azimuth',
	'cosZ',
	'dirPhi',
]

setLogX = {
    'Energy':1,
    'Zenith':0,
    'Azimuth':0,
    'cosZ':0,
    'dirPhi':0,
}
setLogY = {
    'Energy':1,
    'Zenith':0,
    'Azimuth':0,
    'cosZ':0,
    'dirPhi':0,
}

rescale = {
    'Energy':1,
    'Zenith':0,
    'Azimuth':0,
    'cosZ':0,
    'dirPhi':0,
}



extension = '.pdf'


f = ROOT.TFile(fname, 'read')
print (f.IsOpen())

#ROOT.gStyle.SetOptStat(0)
c = ROOT.TCanvas('c','', 2)
for hname in histnames :
    h = f.Get(hname)
    c.SetLogx(setLogX[hname])
    c.SetLogy(setLogY[hname])

    if (rescale[hname]):
        stats = array.array('d',[0.]*10)
        h.GetStats(stats)
        entries = h.GetEntries()
        h.Scale(1.,"width")
        h.SetEntries(entries)
        h.PutStats(stats)

    if (not setLogY[hname]) :
        h.SetMinimum(0.)

    h.Draw("hist")
    c.SaveAs(pref+hname+extension)


f.Close()
