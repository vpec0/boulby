#!/usr/bin/env python
# coding: utf-8

# In[60]:


import sys
import ROOT

sys.path.append('notebooks/')
import MyStyle
MyStyle.set_my_style()


# In[249]:


style = ROOT.gStyle
style.SetPadLeftMargin(0.18)
style.SetPadRightMargin(0.12)
style.SetPadTopMargin(0.15)
style.SetPadBottomMargin(0.15)
style.SetMarkerStyle(ROOT.kFullDotSmall)
style.SetNdivisions(505, 'x')
style.SetTitleOffset(1.2,"xy")
style.SetTitleOffset(1.6, 'y')


# In[61]:


canvases = {}


# In[275]:


fname = 'vitaly_generated_muons/muons-boulby-3002-100m-by250k/*root'

pref = 'plots/full_geom_v1/gen_muons/3002-100m/'
selections = [
    {'pref':'', 'sel':'', 'tit':'All'},
    {'pref':'z_lt_32m_cut_', 'sel':'posZ < 3200', 'tit':'Z < 32 m'},
    {'pref':'z_gt_32m_cut_', 'sel':'posZ > 3200', 'tit':'Z > 32 m'}
]
extension = '.pdf'


tree = ROOT.TChain('gen_muon_tree')
tree.Add(fname)

# # Spatial Distribution

for select in selections :
    outpref = pref + select['pref']
    selection = select['sel']
    title = select['tit']


    c = ROOT.TCanvas('spatial_distro','', 900,300)
    canvases[c.GetName()] = c
    c.Divide(3)

    c.cd(1)
    tree.Draw('posY:posX', selection,'', 1000)
    ROOT.gROOT.ls()

    c.cd(2)
    tree.Draw('posZ:posX', selection,'', 1000)

    c.cd(3)
    tree.Draw('posZ:posY', selection,'', 1000)


    c.Draw()
    c.Print(outpref + c.GetName() + extension)


# # # Angular distro

# # ## individual cosines

# # In[279]:


# c = ROOT.TCanvas('angular_distro_cosines','', 900,300)
# canvases[c.GetName()] = c
# c.Divide(3)

# axes = ['X','Y','Z']

# for i,axis in zip(range(1,4),axes) :
#     c.cd(i).SetGrid()
#     tree.Draw('cos%s>>h%s(100,-1.,1.)'%(axis,axis), selection)
#     h = ROOT.gDirectory.Get('h%s'%axis)
#     h.SetTitle(title+';cos%s'%axis)


# c.Draw()
# c.Print(outpref + c.GetName() + extension)


# # ## Zenith & Azimuth: Z pointing up

# # In[280]:


# c = ROOT.TCanvas('angular_distro_zen_azi','', 800,400)
# canvases[c.GetName()] = c
# c.Divide(2)

# c.cd(1)
# tree.Draw('cosZ>>hzenith(100,-1.,0.)', selection,'')
# h = ROOT.gDirectory.Get('hzenith')
# h.SetTitle(title+';cos(#theta)')

# c.cd(2)
# ROOT.gPad.SetGrid()
# tree.Draw('TMath::ATan2(cosY,cosX)/TMath::Pi()*180>>hazimuth(90,-180,180)', selection,'')
# h = ROOT.gDirectory.Get('hazimuth')
# h.SetTitle(title+';#phi [#circ]')
# h.SetMinimum(0)
# h.SetMaximum(h.GetMaximum() * 1.2)


# c.Draw()
# c.Print(outpref + c.GetName() + extension)


# # ## Zenith & Azimuth: Y pointing up

# # In[281]:


# c = ROOT.TCanvas('angular_distro_zen_azi_rot','', 800,400)
# canvases[c.GetName()] = c
# c.Divide(2)

# c.cd(1)
# tree.Draw('cosY>>hzenith(100,-1.,0.)', selection,'')
# h = ROOT.gDirectory.Get('hzenith')
# h.SetTitle(title+';cos(#theta)')

# c.cd(2)
# tree.Draw('TMath::ATan2(cosZ,cosX)/TMath::Pi()*180>>hazimuth(90,-180,180)', selection,'')
# h = ROOT.gDirectory.Get('hazimuth')
# h.SetTitle(title+';#phi [#circ]')


# c.Draw()
# # c.Print(outpref + c.GetName() + extension)


# # # Energy Distribution

# # In[282]:


# import math, array
# def doLogAxis(hist, ax = 'x') :
#     axis = {'x':hist.GetXaxis(), 'y':hist.GetYaxis()}[ax.lower()]

#     start = math.log10(axis.GetXmin())
#     stop  = math.log10(axis.GetXmax())
#     nbins = axis.GetNbins()
#     binwidth = (stop-start)/nbins

#     bins = [pow(10.,start + i*binwidth) for i in range(nbins+1)]
#     axis.Set(nbins, array.array('d', bins))


# # In[287]:


# c = ROOT.TCanvas('energy_distro','', 400,400)
# canvases[c.GetName()] = c
# c.SetLogx()
# c.SetLogy()

# # create a histogram with log scale
# h = ROOT.TH1F('hEnergyLog', title+';Energy [GeV]; #DeltaN/#DeltaE [GeV^{-1}]', 120, 0.1,1e5)
# doLogAxis(h,'x')

# tree.Draw('energy>>+hEnergyLog', selection, 'hist')

# nentries = h.GetEntries()
# mean = h.GetMean()
# rms = h.GetRMS()
# # print (nentries)
# print(mean)
# print(rms)
# stats = array.array('d',[0.]*10)
# h.GetStats(stats)
# h.Scale(1.,'width')
# h.PutStats(stats)
# h.SetEntries(nentries)
# # h.SetMean(mean)
# # h.SetRMS(rms)
# # nentries = h.GetEntries()
# # print (nentries)
# print(h.GetMean())
# print(h.GetRMS())

# c.Draw()
# c.Print(outpref + c.GetName() + extension)


# # In[ ]:
