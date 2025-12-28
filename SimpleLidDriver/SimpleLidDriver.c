/*
 * SimpleLidDriver.c
 * 最简单的盖子驱动 - 阻止盖子关闭信号
 *
 * 功能: 替换坏掉的 ACPI\ID9001 驱动,永远报告盖子为"开启"状态
 */

#include <ntddk.h>
#include <wdf.h>

// 驱动入口
DRIVER_INITIALIZE DriverEntry;

// 设备添加
EVT_WDF_DRIVER_DEVICE_ADD SimpleLidEvtDeviceAdd;

// 驱动入口点
NTSTATUS
DriverEntry(
    _In_ PDRIVER_OBJECT  DriverObject,
    _In_ PUNICODE_STRING RegistryPath
)
{
    WDF_DRIVER_CONFIG config;
    NTSTATUS status;

    // 打印调试信息
    KdPrint(("SimpleLidDriver: DriverEntry - 驱动加载成功!\n"));

    // 初始化驱动配置
    WDF_DRIVER_CONFIG_INIT(&config, SimpleLidEvtDeviceAdd);

    // 创建驱动对象
    status = WdfDriverCreate(
        DriverObject,
        RegistryPath,
        WDF_NO_OBJECT_ATTRIBUTES,
        &config,
        WDF_NO_HANDLE
    );

    if (!NT_SUCCESS(status)) {
        KdPrint(("SimpleLidDriver: WdfDriverCreate 失败! 错误码: 0x%x\n", status));
        return status;
    }

    KdPrint(("SimpleLidDriver: 驱动初始化完成\n"));
    return STATUS_SUCCESS;
}

// 设备添加回调
NTSTATUS
SimpleLidEvtDeviceAdd(
    _In_ WDFDRIVER       Driver,
    _Inout_ PWDFDEVICE_INIT DeviceInit
)
{
    NTSTATUS status;
    WDFDEVICE device;

    UNREFERENCED_PARAMETER(Driver);

    KdPrint(("SimpleLidDriver: EvtDeviceAdd - 检测到设备\n"));

    // 创建设备对象(最简配置)
    status = WdfDeviceCreate(
        &DeviceInit,
        WDF_NO_OBJECT_ATTRIBUTES,
        &device
    );

    if (!NT_SUCCESS(status)) {
        KdPrint(("SimpleLidDriver: WdfDeviceCreate 失败! 错误码: 0x%x\n", status));
        return status;
    }

    KdPrint(("SimpleLidDriver: 设备创建成功 - 盖子驱动已禁用\n"));

    // 关键: 什么都不做,不响应盖子事件
    // 系统将无法接收到"盖子关闭"信号

    return STATUS_SUCCESS;
}
