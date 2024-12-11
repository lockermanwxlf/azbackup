terraform {
  cloud {
    organization = "lockermanwxlf"
    workspaces {
      name = "azure-backups"
    }
  }
    required_providers {
        azurerm = {
            source = "hashicorp/azurerm"
        }
    }
}

provider "azurerm" {
  features {}
}

resource "azurerm_resource_group" "resource_group" {
  name = "azure_backups"
  location = "eastus"
}

resource "random_string" "uid" {
  length = 16
  lower = true
  numeric = true
  upper = false
  special = false
}

resource "azurerm_storage_account" "storage_account" {
  name = "azbackup${random_string.uid.result}"
  resource_group_name = azurerm_resource_group.resource_group.name

  access_tier = "Hot"
  account_tier = "Standard"
  account_replication_type = "LRS"
  location = azurerm_resource_group.resource_group.location
}

output "storage_account_connection_string" {
  value = azurerm_storage_account.storage_account.primary_connection_string
  sensitive = true
}