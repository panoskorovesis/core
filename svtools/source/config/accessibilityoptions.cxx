/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */


#include <svtools/accessibilityoptions.hxx>

#include <unotools/configmgr.hxx>
#include <com/sun/star/uno/Any.hxx>

#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <comphelper/configurationhelper.hxx>
#include <comphelper/processfactory.hxx>

#include <vcl/settings.hxx>
#include <vcl/svapp.hxx>
#include <tools/diagnose_ex.h>
#include <mutex>

#include "itemholder2.hxx"

using namespace utl;
using namespace com::sun::star::uno;

#define HELP_TIP_TIMEOUT 0xffff     // max. timeout setting to pretend a non-timeout

// class SvtAccessibilityOptions_Impl ---------------------------------------------

class SvtAccessibilityOptions_Impl
{
private:
    css::uno::Reference< css::container::XNameAccess > m_xCfg;
    css::uno::Reference< css::beans::XPropertySet > m_xNode;

public:
    SvtAccessibilityOptions_Impl();

    void        SetVCLSettings();
    bool        GetIsHelpTipsDisappear() const;
    bool        GetIsAllowAnimatedGraphics() const;
    bool        GetIsAllowAnimatedText() const;
    bool        GetIsAutomaticFontColor() const;
    sal_Int16   GetHelpTipSeconds() const;
    bool        IsSelectionInReadonly() const;
    sal_Int16   GetEdgeBlending() const;
    sal_Int16   GetListBoxMaximumLineCount() const;
    sal_Int16   GetColorValueSetColumnCount() const;
    bool        GetPreviewUsesCheckeredBackground() const;
};

// initialization of static members --------------------------------------

SvtAccessibilityOptions_Impl* SvtAccessibilityOptions::sm_pSingleImplConfig =nullptr;
sal_Int32                     SvtAccessibilityOptions::sm_nAccessibilityRefCount(0);

namespace
{
    std::mutex& SingletonMutex()
    {
        static std::mutex SINGLETON;
        return SINGLETON;
    }
}


// class SvtAccessibilityOptions_Impl ---------------------------------------------

SvtAccessibilityOptions_Impl::SvtAccessibilityOptions_Impl()
{
    try
    {
        m_xCfg.set(
            ::comphelper::ConfigurationHelper::openConfig(
                comphelper::getProcessComponentContext(),
                "org.openoffice.Office.Common/Accessibility",
                ::comphelper::EConfigurationModes::Standard ),
            css::uno::UNO_QUERY);
        m_xNode.set(m_xCfg, css::uno::UNO_QUERY);
    }
    catch(const css::uno::Exception&)
    {
        DBG_UNHANDLED_EXCEPTION("svtools.config");
        m_xCfg.clear();
    }
}

bool SvtAccessibilityOptions_Impl::GetIsHelpTipsDisappear() const
{
    bool                                            bRet = true;

    try
    {
        if(m_xNode.is())
            m_xNode->getPropertyValue("IsHelpTipsDisappear") >>= bRet;
    }
    catch(const css::uno::Exception&)
    {
        DBG_UNHANDLED_EXCEPTION("svtools.config");
    }

    return bRet;
}

bool SvtAccessibilityOptions_Impl::GetIsAllowAnimatedGraphics() const
{
    bool                                            bRet = true;

    try
    {
        if(m_xNode.is())
            m_xNode->getPropertyValue("IsAllowAnimatedGraphics") >>= bRet;
    }
    catch(const css::uno::Exception&)
    {
        DBG_UNHANDLED_EXCEPTION("svtools.config");
    }

    return bRet;
}

bool SvtAccessibilityOptions_Impl::GetIsAllowAnimatedText() const
{
    bool                                            bRet = true;

    try
    {
        static constexpr OUStringLiteral PROPNAME = u"IsAllowAnimatedText";
        if(m_xNode.is())
            m_xNode->getPropertyValue(PROPNAME) >>= bRet;
    }
    catch(const css::uno::Exception&)
    {
        DBG_UNHANDLED_EXCEPTION("svtools.config");
    }

    return bRet;
}

bool SvtAccessibilityOptions_Impl::GetIsAutomaticFontColor() const
{
    bool                                            bRet = false;

    try
    {
        if(m_xNode.is())
            m_xNode->getPropertyValue("IsAutomaticFontColor") >>= bRet;
    }
    catch(const css::uno::Exception&)
    {
        DBG_UNHANDLED_EXCEPTION("svtools.config");
    }

    return bRet;
}

sal_Int16 SvtAccessibilityOptions_Impl::GetHelpTipSeconds() const
{
    sal_Int16                                       nRet = 4;

    try
    {
        if(m_xNode.is())
            m_xNode->getPropertyValue("HelpTipSeconds") >>= nRet;
    }
    catch(const css::uno::Exception&)
    {
        DBG_UNHANDLED_EXCEPTION("svtools.config");
    }

    return nRet;
}

bool SvtAccessibilityOptions_Impl::IsSelectionInReadonly() const
{
    bool                                            bRet = false;

    try
    {
        if(m_xNode.is())
            m_xNode->getPropertyValue("IsSelectionInReadonly") >>= bRet;
    }
    catch(const css::uno::Exception&)
    {
        DBG_UNHANDLED_EXCEPTION("svtools.config");
    }

    return bRet;
}

sal_Int16 SvtAccessibilityOptions_Impl::GetEdgeBlending() const
{
    sal_Int16 nRet = 35;

    try
    {
        if(m_xNode.is())
            m_xNode->getPropertyValue("EdgeBlending") >>= nRet;
    }
    catch(const css::uno::Exception&)
    {
        DBG_UNHANDLED_EXCEPTION("svtools.config");
    }

    return nRet;
}

sal_Int16 SvtAccessibilityOptions_Impl::GetListBoxMaximumLineCount() const
{
    sal_Int16 nRet = 25;

    try
    {
        if(m_xNode.is())
            m_xNode->getPropertyValue("ListBoxMaximumLineCount") >>= nRet;
    }
    catch(const css::uno::Exception&)
    {
        DBG_UNHANDLED_EXCEPTION("svtools.config");
    }

    return nRet;
}

sal_Int16 SvtAccessibilityOptions_Impl::GetColorValueSetColumnCount() const
{
#ifdef IOS
    return 4;
#else
    sal_Int16 nRet = 12;

    try
    {
        if(m_xNode.is())
            m_xNode->getPropertyValue("ColorValueSetColumnCount") >>= nRet;
    }
    catch(const css::uno::Exception&)
    {
        DBG_UNHANDLED_EXCEPTION("svtools.config");
    }

    return nRet;
#endif
}

bool SvtAccessibilityOptions_Impl::GetPreviewUsesCheckeredBackground() const
{
    bool bRet = false;

    try
    {
        if(m_xNode.is())
            m_xNode->getPropertyValue("PreviewUsesCheckeredBackground") >>= bRet;
    }
    catch(const css::uno::Exception&)
    {
        DBG_UNHANDLED_EXCEPTION("svtools.config");
    }

    return bRet;
}

void SvtAccessibilityOptions_Impl::SetVCLSettings()
{
    AllSettings aAllSettings(Application::GetSettings());
    StyleSettings aStyleSettings(aAllSettings.GetStyleSettings());
    HelpSettings aHelpSettings(aAllSettings.GetHelpSettings());
    bool StyleSettingsChanged(false);

    aHelpSettings.SetTipTimeout( GetIsHelpTipsDisappear() ? GetHelpTipSeconds() * 1000 : HELP_TIP_TIMEOUT);
    aAllSettings.SetHelpSettings(aHelpSettings);

    const sal_Int16 nEdgeBlendingCountA(GetEdgeBlending());
    OSL_ENSURE(nEdgeBlendingCountA >= 0, "OOps, negative values for EdgeBlending are not allowed (!)");
    const sal_uInt16 nEdgeBlendingCountB(static_cast< sal_uInt16 >(nEdgeBlendingCountA >= 0 ? nEdgeBlendingCountA : 0));

    if(aStyleSettings.GetEdgeBlending() != nEdgeBlendingCountB)
    {
        aStyleSettings.SetEdgeBlending(nEdgeBlendingCountB);
        StyleSettingsChanged = true;
    }

    const sal_Int16 nMaxLineCountA(GetListBoxMaximumLineCount());
    OSL_ENSURE(nMaxLineCountA >= 0, "OOps, negative values for ListBoxMaximumLineCount are not allowed (!)");
    const sal_uInt16 nMaxLineCountB(static_cast< sal_uInt16 >(nMaxLineCountA >= 0 ? nMaxLineCountA : 0));

    if(aStyleSettings.GetListBoxMaximumLineCount() != nMaxLineCountB)
    {
        aStyleSettings.SetListBoxMaximumLineCount(nMaxLineCountB);
        StyleSettingsChanged = true;
    }

    const sal_Int16 nMaxColumnCountA(GetColorValueSetColumnCount());
    OSL_ENSURE(nMaxColumnCountA >= 0, "OOps, negative values for ColorValueSetColumnCount are not allowed (!)");
    const sal_uInt16 nMaxColumnCountB(static_cast< sal_uInt16 >(nMaxColumnCountA >= 0 ? nMaxColumnCountA : 0));

    if(aStyleSettings.GetColorValueSetColumnCount() != nMaxColumnCountB)
    {
        aStyleSettings.SetColorValueSetColumnCount(nMaxColumnCountB);
        StyleSettingsChanged = true;
    }

    const bool bPreviewUsesCheckeredBackground(GetPreviewUsesCheckeredBackground());

    if(aStyleSettings.GetPreviewUsesCheckeredBackground() != bPreviewUsesCheckeredBackground)
    {
        aStyleSettings.SetPreviewUsesCheckeredBackground(bPreviewUsesCheckeredBackground);
        StyleSettingsChanged = true;
    }

    if(StyleSettingsChanged)
    {
        aAllSettings.SetStyleSettings(aStyleSettings);
        Application::MergeSystemSettings(aAllSettings);
    }

    Application::SetSettings(aAllSettings);
}

// class SvtAccessibilityOptions --------------------------------------------------

SvtAccessibilityOptions::SvtAccessibilityOptions()
{
    if (!utl::ConfigManager::IsFuzzing())
    {
        std::unique_lock aGuard( SingletonMutex() );
        if(!sm_pSingleImplConfig)
        {
            sm_pSingleImplConfig = new SvtAccessibilityOptions_Impl;
            aGuard.unlock(); // because holdConfigItem will call this constructor
            svtools::ItemHolder2::holdConfigItem(EItem::AccessibilityOptions);
        }
        ++sm_nAccessibilityRefCount;
    }
    //StartListening( *sm_pSingleImplConfig, sal_True );
}

SvtAccessibilityOptions::~SvtAccessibilityOptions()
{
    //EndListening( *sm_pSingleImplConfig, sal_True );
    std::unique_lock aGuard( SingletonMutex() );
    if( !--sm_nAccessibilityRefCount )
    {
        //if( sm_pSingleImplConfig->IsModified() )
        //  sm_pSingleImplConfig->Commit();
        delete sm_pSingleImplConfig;
        sm_pSingleImplConfig = nullptr;
    }
}

bool SvtAccessibilityOptions::GetIsAllowAnimatedGraphics() const
{
    return sm_pSingleImplConfig->GetIsAllowAnimatedGraphics();
}
bool SvtAccessibilityOptions::GetIsAllowAnimatedText() const
{
    return sm_pSingleImplConfig->GetIsAllowAnimatedText();
}
bool SvtAccessibilityOptions::GetIsAutomaticFontColor() const
{
    return sm_pSingleImplConfig->GetIsAutomaticFontColor();
}
bool SvtAccessibilityOptions::IsSelectionInReadonly() const
{
    return sm_pSingleImplConfig->IsSelectionInReadonly();
}


void SvtAccessibilityOptions::SetVCLSettings()
{
    sm_pSingleImplConfig->SetVCLSettings();
}


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
